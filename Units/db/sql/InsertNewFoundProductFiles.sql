CREATE PROCEDURE [dbo].[InsertNewFoundProductFiles] 
(@XMLInspectionData nvarchar(max), @InspectionGUID uniqueidentifier, @AddingDate  Datetime)
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

declare @ProductFileAmount int;
declare @I int;
declare @MaxFileID int;
declare @FoundProductFileContainerGUID uniqueidentifier;
declare @FileGUID uniqueidentifier;
--declare @AddFoundFilesInLibThenInspecting bit;
declare @LibFileCount int;
declare @SoftTypeGUID uniqueidentifier;

declare @OriginalFileName nvarchar(450);
declare @FileNameAndPath nvarchar(1000);
declare @FileName nvarchar(1000);
declare @FileVersion nvarchar(450);
declare @Company nvarchar(450);
declare @ProductName nvarchar(450);
declare @ProductVersion nvarchar(450);
declare @Language nvarchar(450);
declare @Size  bigint;

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
    
        
	 CREATE TABLE #XMLProductFileTable(
		[ID]                 [int] IDENTITY(1,1) NOT NULL,
		[FileName]           [nvarchar](300)  collate Cyrillic_General_CI_AS NULL,
		[FileVersion]        [nvarchar](450)  collate Cyrillic_General_CI_AS NULL,
		[OriginalFileName]   [nvarchar](450)  collate Cyrillic_General_CI_AS NULL,  
		[Company]            [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
		[ProductName]        [nvarchar](450)  collate Cyrillic_General_CI_AS NULL, 
		[ProductVersion]     [nvarchar](450) collate Cyrillic_General_CI_AS NULL,  
		[Language]           [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
		[Size]               [bigint]
	 ) ON [PRIMARY]; 
	     
	     
	insert into #XMLProductFileTable                
	select *
	from openxml(@XML, '/ResearchInfo/ProductFile/FileInfo', 1)
	with (FileName nvarchar(300),
		  FileVersion nvarchar(450),
		  OriginalFileName nvarchar(450),  
		  Company nvarchar(450),
		  ProductName nvarchar(450), 
		  ProductVersion nvarchar(450),  
		  Language nvarchar(450),
		  Size  bigint);                   
          
                
	 select @ProductFileAmount = Count(*) from #XMLProductFileTable;

	 if (@ProductFileAmount >0)
	 begin  

		   select @MaxFileID = max(ID) from #XMLProductFileTable;

		   select @FoundProductFileContainerGUID = ObjectGUID from ObjectRelations where ObjectType = 'FoundProductFileContainer';
		   
		   Update #XMLProductFileTable
		   set OriginalFileName  = RIGHT(FileName, CHARINDEX('\', REVERSE(FileName))-1)
		   where OriginalFileName = '' or OriginalFileName is NULL;
	       
		   set @I = 1;

		   while (@I <= @MaxFileID)
		   begin
	        
				   set @FileGUID = NULL;

                   select @OriginalFileName = XF.OriginalFileName, @FileNameAndPath = XF.FileName,
                          @FileName = RIGHT(XF.FileName, CHARINDEX('\', REVERSE(XF.FileName))-1),  
				          @FileVersion = XF.FileVersion, @Company = XF.Company, 
				          @ProductName = XF.ProductName, @ProductVersion = XF.ProductVersion, 
				          @Language = XF.Language, @Size = XF.Size 
                   from #XMLProductFileTable XF
				   where  XF.ID = @I;

				   select @FileGUID = F.ObjectGUID  from FoundProductFile F
				   where  F.OriginalFileName = @OriginalFileName and F.FileName = @FileName 
				          and F.FileVersion = @FileVersion and F.Company = @Company 
				          and F.ProductName = @ProductName and F.ProductVersion = @ProductVersion 
				          and F.Language = @Language  and F.Size = @Size;
	      
				   if (@FileGUID is NULL)
				   begin

						set @FileGUID = NEWID();                        

						insert into dbo.FoundProductFile
						values (@FileGUID, @FoundProductFileContainerGUID, @FileName,  @FileVersion, 
								@OriginalFileName, @Company, @ProductName, @ProductVersion, @Language, @Size);
	                     
						if (@@Error <> 0)   set @Check = -1; 
	     
				   end 
                                
				   insert into dbo.FileInstallation 
				   values (NEWID(), @InspectionGUID, @FileGUID, @FileNameAndPath); 
				   if (@@Error <> 0)   set @Check = -1;                       
	                                     
				   
				   -- adding found file in library
				   --if(@AddFoundFilesInLibThenInspecting = 1)
				  -- begin
				         
                         --do exist such soft? 
                         set @SoftTypeGUID = NULL;
                             
				         select @SoftTypeGUID = ST.ObjectGUID
	                     from SoftType ST, Publisher P
	                     where @Company = P.Name and ST.ProductName = @ProductName and ST.PublisherGUID = P.ObjectGUID
							  and ST.VersionMajor = dbo.GetMajorVersionFromFullVersion(@ProductVersion);

                         if(@SoftTypeGUID is not NULL)
                         begin

                               -- do exist lib file with properties as found file?
								 select @LibFileCount = count(*) 
								 from LibProductFile LF
								 where @OriginalFileName = LF.OriginalFileName and LF.ParentGUID = @SoftTypeGUID
								       and @FileVersion = LF.FileVersion  
									   and @Language = LF.Language and @Size = LF.Size;

                                 if(@LibFileCount < 1)
								 begin
                                        insert into dbo.LibProductFile 
										values (NEWID(),  @SoftTypeGUID, @FileVersion, @OriginalFileName,  
											   @Company, @ProductName, @ProductVersion, @Language, @Size, 0);
								 end 
                         end


				   --end

                   set @I = @I+1;
				   
			end

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
    Set @Message2 = 'Ошибка добавления новых исполняемых файлов в БД: ';
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


