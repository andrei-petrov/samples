CREATE PROCEDURE [dbo].[InsertNewSoft] 
(@XMLInspectionData nvarchar(max), @InspectionGUID uniqueidentifier, @Date  Datetime, @InspectionType smallint, @AddingDate  Datetime)
-- Adding installed soft information from XML 
-- For execution only from  SP InsertNewInspectionData
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

declare @XML int;

declare @Message  nvarchar(1000);
declare @Message1  nvarchar(500);
declare @Message2  nvarchar(500);

declare @MaxSoftID int;
declare @Length int;
declare @Count int;
declare @SoftAmount int;
declare @HardwareAmount int;
declare @ProductFileAmount int
declare @I int;
declare @InstallationObjID int;
declare @InstallDate datetime;
declare @CharInstallDate nvarchar(100);
declare @InspectionCount int;
declare @SoftCount int;
declare @PublisherContainerGUID uniqueidentifier;
declare @UndefinedCategoryGUID uniqueidentifier;
declare @SoftTypeContainerGUID uniqueidentifier;
declare @VersionMajor nvarchar(100);
declare @VersionMinor nvarchar(100);
declare @DisplayName  nvarchar(450);

declare @SoftGUID uniqueidentifier;
declare @SoftTypeGUID uniqueidentifier;
declare @CategoryGUID uniqueidentifier;
declare @PublisherGUID uniqueidentifier;
declare @SoftContainerGUID  uniqueidentifier;
declare @DefaultPublisher  nvarchar(450);
declare @DefaultProduct  nvarchar(450);

declare @PublisherName nvarchar(450);
declare @ProductName nvarchar(450);
declare @IsOperatingSystem smallint;
declare @Mark  nvarchar(500);

-- for  transaction
declare @Check int;
set @Check = 0;

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

--select @UndefinedCategoryGUID = ObjectGUID from Category
--where  Name = 'ПО без категории';

select @SoftContainerGUID = ObjectGUID from ObjectRelations where ObjectType = 'SoftContainer';
select @PublisherContainerGUID = ObjectGUID from ObjectRelations where ObjectType = 'PublisherContainer';
select @SoftTypeContainerGUID = ObjectGUID from ObjectRelations where ObjectType = 'SoftTypeContainer';


   --------    reading a data of the installed soft  from XML into help table   --------

   ----------------------------------------------  OS   -----------------------------------
   CREATE TABLE #XMLSoftHelpTable(
          [ID] [int] IDENTITY(1,1) NOT NULL,
         [Publisher] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
         [VersionMinor] [nvarchar](50) collate Cyrillic_General_CI_AS NULL,
         [VersionMajor] [nvarchar](50) collate Cyrillic_General_CI_AS NULL,
         [Language] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
         [ProductName] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
         [ObjectGUID] [uniqueidentifier] NULL,
         [IsOperatingSystem] [smallint] NULL,
         [ProductKey] [nvarchar](50) collate Cyrillic_General_CI_AS NULL,
         [InstallDate] [nvarchar](100)  NULL,    --  nvarchar in order to not fall from incorrect date
         [PublisherGUID] [uniqueidentifier] NULL,
         [SoftTypeGUID] [uniqueidentifier] NULL, 
		 [VersionBuildMinor] [nvarchar](150) collate Cyrillic_General_CI_AS NULL,
         [VersionBuildMajor] [nvarchar](150) collate Cyrillic_General_CI_AS NULL,  
     ) ON [PRIMARY]; 

	------------------------------------------------------     
	-- Attention! 
	-- Columns listed below should correspond to the fields 
	-- in GetComputerOS stored procedure.
	-- (except fields which are not contained in xml and have an appropriate comment)               
	-- So if you change this list somehow
	-- you should also change it in  GetComputerOS.sql,
	-- which is needed to export data from db to xml.
	------------------------------------------------------                    
	insert into #XMLSoftHelpTable                
	select *
	from openxml(@XML, '/ResearchInfo/OS/System', 1)
		with (Publisher nvarchar(450),
			VersionMinor nvarchar(50),
			VersionMajor nvarchar(50),
			Language nvarchar(450),                          
			ProductName nvarchar(450),
			ObjectGUID nvarchar(50),         -- not present in xml. will be filled later.
			IsOperatingSystem smallint,      -- not present in xml. will be filled later.
			ProductKey  nvarchar(50),
			InstallDate nvarchar(100),       --  nvarchar in order to not fall from incorrect date
			PublisherGUID nvarchar(50),
			SoftTypeGUID nvarchar(50),      -- not present in xml. will be filled later.); 
			VersionBuildMinor nvarchar(150),
			VersionBuildMajor nvarchar(150)); 
	        
	update #XMLSoftHelpTable
	set IsOperatingSystem = 1;

	update #XMLSoftHelpTable
	set VersionBuildMinor = '' where VersionBuildMinor is NULL;

    update #XMLSoftHelpTable
	set VersionBuildMajor = '' where VersionBuildMajor is NULL;
        
-----------------------------------------  Installed Soft    --------------------------- 
       -- Attention! 
       -- Columns listed below should correspond to the fields 
       -- in GetComputerSoft stored procedure.
       -- (except fields which are not contained in xml and have an appropriate comment)
       -- So if you change this list somehow
       -- you should also change it in  GetComputerSoft.sql,
       -- which is needed to export data from db to xml.
       ------------------------------------------------------     

    insert into #XMLSoftHelpTable                
    select *
    from openxml(@XML, '/ResearchInfo/Soft/InstallInfo', 1)
      with (Publisher nvarchar(450),          
            VersionMinor nvarchar(20),
            VersionMajor nvarchar(20),
            Language nvarchar(450),                          
            ProductName nvarchar(450),
            ObjectGUID nvarchar(50),     -- not present in xml. will be filled later.             
            IsOperatingSystem smallint,  -- not present in xml. will be filled later.             
            ProductKey  nvarchar(50),                  
            InstallDate nvarchar(100),       --  nvarchar in order to not fall from incorrect date
            PublisherGUID nvarchar(50),
            SoftTypeGUID nvarchar(50),     -- not present in xml. will be filled later.); 
			VersionBuildMinor nvarchar(150),
			VersionBuildMajor nvarchar(150));                      

	update #XMLSoftHelpTable
	set IsOperatingSystem = 0
	where  IsOperatingSystem is NULL;
 
	select @SoftAmount = Count(*) from #XMLSoftHelpTable;

	if (@SoftAmount <1 or @SoftAmount is null)
	begin
	      RAISERROR ('Список установленного ПО пуст ', 12, 1);
	end
	else
	begin  
                 
		select @DefaultPublisher = DefaultPublisher from ObjectRoot;
		select @DefaultProduct = DefaultProductName from ObjectRoot;

		update #XMLSoftHelpTable
		set Publisher = @DefaultPublisher
		where Publisher is NULL or Publisher= '';

		update #XMLSoftHelpTable
		set ProductName = @DefaultProduct
		where ProductName is NULL or ProductName = '';

		update #XMLSoftHelpTable
		set ProductName = dbo.GetProductNameWithoutLanguage(ProductName);

		update #XMLSoftHelpTable
		set Publisher = dbo.GetNameFromTemplate(Publisher, 'Publisher'),
			ProductName = dbo.GetNameFromTemplate(ProductName, 'ProductName');

		update #XMLSoftHelpTable
		set ProductName = dbo.GetProductNameGroup(ProductName)
		where IsOperatingSystem = 0 and (VersionMinor <> '' or VersionMajor <> '');


		update #XMLSoftHelpTable
		set Publisher = LTRIM(Publisher), ProductName = LTRIM(ProductName);

		update #XMLSoftHelpTable
		set Publisher = RTRIM(Publisher), ProductName = RTRIM(ProductName);


		update #XMLSoftHelpTable
		set Publisher = REPLACE(Publisher, ';', ','), ProductName = REPLACE(ProductName, ';', ',');

		update #XMLSoftHelpTable
		set Publisher = REPLACE(Publisher, '[', '('), ProductName = REPLACE(ProductName, '[', '(');

		update #XMLSoftHelpTable
		set Publisher = REPLACE(Publisher, ']', ')'), ProductName = REPLACE(ProductName, ']', ')');
		
		update #XMLSoftHelpTable
		set Publisher = Replace(Replace(Replace(Publisher, ' Corp.', ' Corporation'),
                                                                ', Inc.', ' Incorporated'),
                                                                ' Inc.', ' Incorporated');


		select @MaxSoftID = max(ID) from #XMLSoftHelpTable;

		set @I = 1;

		while (@I <= @MaxSoftID)
		begin

		---- looking for PublisherGUID, if non exist - create

					set @PublisherGUID = NULL;

					select @PublisherGUID = P.ObjectGUID from Publisher P, #XMLSoftHelpTable XS
					where P.Name = XS.Publisher and XS.ID = @I;

					if (@PublisherGUID is NULL)
					begin
						 select @PublisherName = Publisher from #XMLSoftHelpTable
						 where ID = @I;

						 set @PublisherGUID = NEWID();                         

						 insert into dbo.Publisher  values(@PublisherGUID, @PublisherContainerGUID, @PublisherName, '', '', '', 0);
						 if (@@Error <> 0)   set @Check = -1; 

					end 

                    Update #XMLSoftHelpTable set PublisherGUID = @PublisherGUID
                    where ID = @I; 
                              

					---- looking for SoftTypeGUID

					set @SoftTypeGUID = NULL;

					select @SoftTypeGUID  =   ST.ObjectGUID  from   SoftType ST, #XMLSoftHelpTable XS
					where ST.ProductName = XS.ProductName and  ST.PublisherGUID = @PublisherGUID
						 and ST.VersionMajor = XS.VersionMajor and XS.ID = @I;

					if (@SoftTypeGUID is NULL)
					begin

						 set @SoftTypeGUID = NEWID();
						 set @CategoryGUID = NULL;
					     
						 select @CategoryGUID = CategoryGUID from SoftType ST, #XMLSoftHelpTable XS
						 where ST.PublisherGUID = @PublisherGUID and ST.ProductName = XS.ProductName
							   and XS.ID = @I;
					           
						 --if (@CategoryGUID is NULL)
							--set @CategoryGUID =  @UndefinedCategoryGUID;     
						     
						 insert into dbo.SoftType
						 select @SoftTypeGUID, @SoftTypeContainerGUID, @PublisherGUID, ProductName, VersionMajor, 
								@CategoryGUID, 0
						 from #XMLSoftHelpTable
						 where ID = @I;                        

						 if (@@Error <> 0)   set @Check = -1; 

					end

                    Update #XMLSoftHelpTable set SoftTypeGUID = @SoftTypeGUID
                    where ID = @I;  

					-----------------------------------------------------                            

					set @SoftGUID = NULL;

					select @SoftGUID = S.ObjectGUID  from Soft S, #XMLSoftHelpTable XS
					where S.SoftTypeGUID = XS.SoftTypeGUID and S.Localization = XS.Language 
						 and S.IsOperatingSystem = XS.IsOperatingSystem and S.VersionMinor = XS.VersionMinor 
						 and ( S.VersionBuildMajor = XS.VersionBuildMajor or S.VersionBuildMajor is NULL and  XS.VersionBuildMajor is NULL)
						 and ( S.VersionBuildMinor = XS.VersionBuildMinor or S.VersionBuildMinor is NULL and  XS.VersionBuildMinor is NULL)
						 and S.ProductKey = XS.ProductKey and XS.ID = @I;

					if (@SoftGUID is NULL)
					begin

						 set @SoftGUID = NEWID();                        

						 insert into dbo.Soft
						 select @SoftGUID, @SoftContainerGUID, @SoftTypeGUID, VersionMinor, Language,
						        ProductKey, IsOperatingSystem, @Date, 1, VersionBuildMinor, VersionBuildMajor
						 from #XMLSoftHelpTable
						 where ID = @I;
					     
						 if (@@Error <> 0)   set @Check = -1; 

					end 

					select @CharInstallDate = InstallDate
					from #XMLSoftHelpTable
					where ID = @I; 
					
					if(isdate(@CharInstallDate) = 1)
						set @InstallDate = @CharInstallDate;
					else
						set @InstallDate = NULL; 

								
					-- adding in Installation
					IF NOT EXISTS(select top(1) * from dbo.Installation where SoftGUID = @SoftGUID and ParentGUID = @InspectionGUID)
					begin
					    
						insert into dbo.Installation values(NEWID(), @SoftGUID, @InspectionGUID, @AddingDate, @InstallDate); 
						if (@@Error <> 0)   set @Check = -1;                       
					        
					end    
					    
					set @I = @I+1;

		   end
   end		   
                        
---------------------------------------------   OS List for computer   -----------------------------------------                        
---------------------------------    List exist only to inspection whith agent    ------------------------------------------------------
          if (@InspectionType = 1 or @InspectionType = 2)
          begin   
 
               CREATE TABLE #XMLOSListTable(
	                     [ID] [int] IDENTITY(1,1) NOT NULL,
	                     [DisplayName] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
                     ) ON [PRIMARY];  
                                   
                insert into #XMLOSListTable                
                select *
                from openxml(@XML, '/ResearchInfo/OS_LIST/OS', 1)
                  with (DisplayName nvarchar(450));                            
                     
                select @MaxSoftID = max(ID) from #XMLOSListTable;
                set @I = 1;     
                set @DisplayName = '';
        
                while (@I <= @MaxSoftID)
                begin
                     select @DisplayName = @DisplayName + '; ' + DisplayName 
                     from #XMLOSListTable  where ID = @I;                                         
                                 
                     set @I = @I+1;
                end 
                
                if (substring(@DisplayName, 1, 1)= ';')
				begin
				set @Length = LEN(@DisplayName) - 1;
				set @DisplayName = Right(@DisplayName, @Length);
				end  

				Update dbo.Action set BootIni = @DisplayName
				where ObjectGUID =  @InspectionGUID; 
				if (@@Error <> 0)   set @Check = -1; 
          end   


if (@Check <> 0)
begin
    Set @Message = 'Ошибка при добавлении записи в таблицу ';
    RAISERROR(@Message, 12, 1);
end

END TRY
BEGIN CATCH

    EXEC sp_xml_removedocument @XML;
    Set @Message1 = Error_Message();
    Set @Message2 = 'Ошибка добавления нового ПО в БД: ';
    Set @Message = @Message2 + @Message1;
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
