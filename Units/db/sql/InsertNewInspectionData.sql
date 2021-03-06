CREATE PROCEDURE [dbo].[InsertInspectionData] 
(@ObjectGUID nvarchar(50), @XMLInspectionData nvarchar(max), @InspectionType smallint)
-- if @InspectionType = 2, @ObjectGUID  is the task guid; else - @ObjectGUID is the computer guid
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

declare @XML int;
declare @Date  Datetime;
declare @CompGroupName  nvarchar(450);
declare @Message  nvarchar(1000);
declare @Message1  nvarchar(500);
declare @Message2  nvarchar(500);
declare @CompName  nvarchar(450);
declare @Domain  nvarchar(300);
declare @Description  nvarchar(300);
declare @CompDescriptionInDB  nvarchar(300);
declare @Virtuality  nvarchar(300);
declare @CompGUID nvarchar(50);
declare @LinkGUID nvarchar(50);
declare @LinkCount int;

declare @AddingDate datetime;
declare @InspectionCount int;
declare @InspectionGUID uniqueidentifier;
declare @IncrementalSearch bit;
declare @IncrementalSearchChar nvarchar(50);

declare @IP  nvarchar(300);
declare @Architecture nvarchar(100);
declare @Users  nvarchar(max);
declare @L bigint;
declare @L_begin bigint;
declare @L_end bigint;
declare @Mark  nvarchar(500);
declare @MarkString  nvarchar(500);
declare @FileSearchXML  nvarchar(4000);
declare @FileSearch bit;
declare @ProgramSearch bit;

declare @ComputerLinkXML  nvarchar(max);

-- for  transaction
declare @Check int;
set @Check = 0;

-- Creating of help document @XML,  reading a data of the researched computer from XML  --
-------------------------------------------------------------------------------------------------------
SET DATEFORMAT ymd;

BEGIN TRY

    set @XMLInspectionData = RTRIM(@XMLInspectionData);

    EXEC sp_xml_preparedocument @XML OUTPUT, @XMLInspectionData;

END TRY
BEGIN CATCH

    Set @Message = Error_Message();
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH


BEGIN TRY

    select * into #XMLInstalInfoHeaderTable
    from openxml(@XML, '/ResearchInfo', 1)
    with (Date  Datetime,
          DomainName  nvarchar(300),
          Description nvarchar(300),
          ComputerName nvarchar(450),
          IP nvarchar(300),
          Mark nvarchar(500),
          Virtuality nvarchar(300));               

    
    update #XMLInstalInfoHeaderTable
    set @Domain = #XMLInstalInfoHeaderTable.DomainName;
    update #XMLInstalInfoHeaderTable
    set @CompName = #XMLInstalInfoHeaderTable.ComputerName;               --   computer name
    update #XMLInstalInfoHeaderTable
    set @Description = #XMLInstalInfoHeaderTable.Description;             --   computer description
    update #XMLInstalInfoHeaderTable
    set @IP = #XMLInstalInfoHeaderTable.IP;
    update #XMLInstalInfoHeaderTable
    set @Mark = #XMLInstalInfoHeaderTable.Mark;
    update #XMLInstalInfoHeaderTable
    set @Virtuality = #XMLInstalInfoHeaderTable.Virtuality;

     select * into #XMLComputerInfoHelpTable                
     from openxml(@XML, '/ResearchInfo/OS/System', 1)
     with (Architecture nvarchar(100));      

     update #XMLComputerInfoHelpTable
     set @Architecture = #XMLComputerInfoHelpTable.Architecture;      

     update #XMLInstalInfoHeaderTable
     set @Date = nullif(#XMLInstalInfoHeaderTable.Date, '');                      --   researching date

END TRY
BEGIN CATCH

    EXEC sp_xml_removedocument @XML;
    Set @Message1 = Error_Message();
    Set @Message2 = 'Ошибка при чтении данных об исследуемом компьютере: ';
    Set @Message = @Message2 + @Message1;
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

    
BEGIN TRY


    if (@Date = '' or @Date is NULL)
    begin
        Set @Message = 'Не указана дата обследования компьютера ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end

    if ( @ObjectGUID is  NULL   or @ObjectGUID = '')
    begin
        
        set @Message1 = @InspectionType;
        if (@InspectionType = 2)
           set  @Message2 = ' таска';
        else   
           set  @Message2 = ' компьютера';
            
        Set @Message = 'Не указан GUID ' + @Message2 + ' при типе инспектирования ' + @Message1;
        RAISERROR(@Message, 12, 1);
        Return -1;
    end
    
    if ( @InspectionType = 0 or @InspectionType = 1 )
        set @CompGUID = @ObjectGUID;
    else
    begin
          select @LinkCount = count(*) from ComputerLink L
          inner join  Computer C on L.ComputerGUID = C.ObjectGUID
          where L.ParentGUID = @ObjectGUID and  C.Name = @CompName; 
          
          -- if not exist - create computer and computerlink
          if (@LinkCount = 0) 
          begin       
				  set @LinkGUID = NEWID();
	                          
				  set @ComputerLinkXML = '<Object ObjectGUID="' + @LinkGUID + '" ParentGUID="' + @ObjectGUID 
										 + '" Name="' + @CompName + '" Description="' + @Description + '" />';
	                                     
				  exec dbo.CreateComputerLink  @ComputerLinkXML; 
				  if (@@Error <> 0)   
                     RAISERROR('Ошибка при добавлении компьютера ', 12, 1); 
                 		  
		  end

          select @CompGUID = ObjectGUID from Computer
          where Name =  @CompName;

    end   
       
    select @InspectionCount = count(*) from  Action 
    where ActionDate = @Date    and ActionType = 1  and ParentGUID = @CompGUID; 

    if (@InspectionCount < 1)
    begin
            BEGIN TRY

            BEGIN TRANSACTION 

            set @AddingDate = getutcdate();
	        set @InspectionGUID = NEWID();

            -----   extract Users  -------------------------------------------------------------

			set @Users = '';

			set @L_begin = CHARINDEX('<Users>', @XMLInspectionData);
			set @L_end = CHARINDEX('</Users>', @XMLInspectionData);

			if (@L_begin > 0 and @L_end > @L_begin)
			begin
				  set @Users = SUBSTRING(@XMLInspectionData, @L_begin, @L_end - @L_begin + 8);
			end

			----------------------------------------------------------------------------------------- 
			-- cut the file searching information xml
			
			set @MarkString = 'USN_Journal_Data';
			set @L_begin = 0;
			set @L_end = 0;
			
			set @L_begin = CHARINDEX(@MarkString, @XMLInspectionData);
			if (@L_begin > 0)
			begin
			       set @L_end = CHARINDEX(@MarkString, @XMLInspectionData, @L_begin + 17);
			       if(@L_end > 0)
			           set @FileSearchXML = SUBSTRING (@XMLInspectionData , @L_begin-1, @L_end - @L_begin +18);			       
			end


		    -----------------  incremental search -------------------------------------------------
		              
			select * into #XMLIncrementalSearchTable
			from openxml(@XML, '/ResearchInfo/ProductFile', 1)
			with (IncrementalSearch nvarchar(100)); 

            set @IncrementalSearch = 0;

			select @IncrementalSearchChar = IncrementalSearch from   #XMLIncrementalSearchTable;

			if  (@IncrementalSearchChar = 'true')
                set @IncrementalSearch = 1;
		                          

		   ---------------------- File and Program search -----------------------------------------

		   set @FileSearch = 0;
           set @ProgramSearch = 0;

		   if(charindex('SearchInfo SearchPath', @XMLInspectionData) > 0)
		       set @FileSearch = 1;
           
		   if(charindex('ProductFile SearchPath', @XMLInspectionData) > 0)
		       set @ProgramSearch = 1;

		   ---------------------------------------------------------------------------------------

			insert into dbo.Action values(@InspectionGUID, 1, @Date, 1, @Mark, @InspectionType, @CompGUID, @AddingDate, @IP, @Architecture, @Domain, @Users, @Virtuality, '', '', @FileSearchXML, @IncrementalSearch, @FileSearch, @ProgramSearch);
			if (@@Error <> 0)   set @Check = -1; 


			exec dbo.InsertNewSoft @XMLInspectionData, @InspectionGUID, @Date, @InspectionType, @AddingDate;
            if (@@Error <> 0)   set @Check = -1;

			exec dbo.InsertNewHardware  @XMLInspectionData, @InspectionGUID, @Date, @InspectionType, @AddingDate;
            if (@@Error <> 0)   set @Check = -1;

			exec dbo.InsertNewFilesFound  @XMLInspectionData, @InspectionGUID, @AddingDate;
            if (@@Error <> 0)   set @Check = -1;

			exec dbo.InsertNewFoundProductFiles @XMLInspectionData, @InspectionGUID, @AddingDate;
            if (@@Error <> 0)   set @Check = -1;
            
            exec dbo.InsertNewUSBDevice  @XMLInspectionData, @InspectionGUID;
            if (@@Error <> 0)   set @Check = -1;


            if(@InspectionType = 2)
            begin
                    if(@Description <> '')
					      update dbo.Computer set Description =  @Description where ObjectGUID = @CompGUID;
					if (@@Error <> 0)   set @Check = -1;
            end
            else 
            begin
                    select @CompDescriptionInDB = Description from Computer where ObjectGUID = @CompGUID;
                     
                    if(@CompDescriptionInDB = '' or @CompDescriptionInDB is NULL) 
					     update dbo.Computer set Description =  @Description where ObjectGUID = @CompGUID;
					if (@@Error <> 0)   set @Check = -1;
            end
         

             if (@Check = 0)
                  COMMIT TRANSACTION
             else 
             begin 
                  Set @Message = 'Ошибка при добавлении строк в таблицы. ';
                  RAISERROR(@Message, 12, 1);
             end

            END TRY
            BEGIN CATCH

            Set @Message1 = Error_Message();
            Set @Message2 = 'Ошибка при добавлении данных в БД. Транзакция отменена ';
            Set @Message = @Message2 + @Message1;
            ROLLBACK TRANSACTION
            RAISERROR(@Message, 12, 1);
            Return -1;

            END CATCH

     end


END TRY
BEGIN CATCH

     EXEC sp_xml_removedocument @XML;
     Set @Message = Error_Message();
     RAISERROR(@Message, 12, 1);
     Return -1;

END CATCH

BEGIN TRY

    EXEC sp_xml_removedocument @XML;

END TRY
BEGIN CATCH

    Set @Message = Error_Message();
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END