CREATE PROCEDURE [dbo].[InsertNewHardware] 
(@XMLInspectionData nvarchar(max), @InspectionGUID uniqueidentifier, @Date  Datetime, @InspectionType smallint, @AddingDate  Datetime)
-- Adding installed hardware information from XML 
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

declare @I int;
declare @N int;
declare @L int;
declare @InstallationObjID int;
declare @Mark  nvarchar(500);

declare @MaxHardwareID int;
declare @Count int;
declare @HardwareAmount int;
declare @InspectionCount int;
declare @VendorCount int;
declare @CodeNameCount int;

declare @HardwareGUID uniqueidentifier;
declare @HardwareContainerGUID  uniqueidentifier;
declare @DefaultUnitName  nvarchar(450);
declare @DefaultManufacturer  nvarchar(450);
declare @DefaultClass  nvarchar(450);
declare @Memory int;
declare @FreeMemory int;
declare @Frequency int;
declare @Quantity  int;
declare @Speed int;
declare @Capacity nvarchar(100);
declare @MemoryDeviceDesc nvarchar(450);
declare @FormFactor nvarchar(100);
declare @MemoryType nvarchar(100);
declare @ProcessorName nvarchar(450);
declare @ChipsetName nvarchar(150);
declare @ChipsetNameList nvarchar(450);
declare @ChipsetCodename nvarchar(150);
declare @ChipsetCodenameList nvarchar(450);

declare @Class  nvarchar(450);
declare @DeviceDesc  nvarchar(450);
declare @MemoryDesc  nvarchar(300);
declare @FriendlyName  nvarchar(450);
declare @Mfg  nvarchar(500);
declare @DisplayName  nvarchar(450);
declare @SerialNumber nvarchar(1000);
declare @SystemUUID uniqueidentifier;
declare @WakeUpType nvarchar(500);
declare @CharReleaseDate nvarchar(100);
declare @ReleaseDate datetime;
declare @Socket nvarchar(500);
declare @ChipsetDesc  nvarchar(450);


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


-----------------------------------------------  Installed Hardware   -----------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------
	------------------------------------------------------     
	-- Attention! 
	-- Columns listed below should correspond to the fields 
	-- in GetComputerHard stored procedure.
	-- So if you change this list somehow
	-- you should also change it in  GetComputerHard.sql,
	-- which is needed to export data from db to xml.
	------------------------------------------------------  

	CREATE TABLE #XMLHardwareHelpTbl(
			  [ID] [int] IDENTITY(1,1) NOT NULL,
			  [ObjectGUID] [uniqueidentifier] NULL,
			  [Class] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
			  [DeviceDesc] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
			  [FriendlyName] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
			  [Mfg] [nvarchar](500) collate Cyrillic_General_CI_AS NULL,
			  [Memory] [bigint] NULL,
			  [FreeMemory] [bigint] NULL,
			  [Frequency] [int] NULL,
			  [Quantity] [int] NULL,
			  [Speed] [int] NULL,
			  [SerialNumber] [nvarchar](500) collate Cyrillic_General_CI_AS NULL,
			  [Version]	[nvarchar](500) collate Cyrillic_General_CI_AS NULL,
			  [UniversalUniqueID] [uniqueidentifier] NULL,
			  [WakeupType] [nvarchar](500) collate Cyrillic_General_CI_AS NULL,
			  [BusWidth] [nvarchar] (500) collate Cyrillic_General_CI_AS  NULL,
			  [BusLength] [nvarchar] (500) collate Cyrillic_General_CI_AS  NULL,
			  [ReleaseDate] [nvarchar](100)  NULL,    --  nvarchar in order to not fall from incorrect date
			  [CurrentUsage] [nvarchar](100) NULL,
			  [MAC] [nvarchar](450),
			  [Socket] [nvarchar](500) collate Cyrillic_General_CI_AS NULL,
			  [Service] [nvarchar](100) collate Cyrillic_General_CI_AS NULL,
              [Chipset] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL
		 ) ON [PRIMARY]; 
	     
	insert into #XMLHardwareHelpTbl                
	select *
	from openxml(@XML, '/ResearchInfo/Hardware/DeviceInfo', 1)
	with (  ObjectGUID  uniqueidentifier,
			Class  nvarchar(450),
			DeviceDesc  nvarchar(450),
			FriendlyName  nvarchar(450),
			Mfg  nvarchar(500),
			Memory   bigint,                                -- this and following not present in xml. will be filled later.                                 
			FreeMemory   bigint,                          
			Frequency   int,                                   
			Quantity     int,                               
			Speed   int,                               
			SerialNumber nvarchar(500),
			Version	nvarchar(500),
			UniversalUniqueID uniqueidentifier,
			WakeupType nvarchar(500),
			BusWidth nvarchar(500),
			BusLength nvarchar(500),
			ReleaseDate nvarchar(100), 
			CurrentUsage nvarchar(100),
			MAC nvarchar(450),
			Socket nvarchar(500),
			Service nvarchar(100),
            Chipset nvarchar(450));              


    update #XMLHardwareHelpTbl
	set DeviceDesc = FriendlyName
	where FriendlyName <> '';


	------------------  Memory  --------------------------------------------


	select * into #XMLSystemInfoMemorySizeTbl
	from openxml(@XML, '/ResearchInfo/SystemInfo/Hardware/TotalMemory', 1)
	with (  DeviceDesc nvarchar(450),
			Value  int);  

	insert into #XMLHardwareHelpTbl                
	select  NULL,  'MEMORY', DeviceDesc, '', '' , Value, NULL, NULL, NULL, NULL, NULL,
	        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
	from #XMLSystemInfoMemorySizeTbl;


	------------------  Old baseboard, temporary  --------------------------------------------

	select *  into #XMLSystemInfoOldBIOSTbl 
	from openxml(@XML, '/ResearchInfo/SystemInfo/Hardware/BIOS', 1)
	with (  BaseBoardProduct  nvarchar(500),
			BaseBoardManufacturer  nvarchar(500));   

	Update #XMLSystemInfoOldBIOSTbl 
	set BaseBoardManufacturer = ' (' +BaseBoardManufacturer + ')'
	where BaseBoardManufacturer <> '' and BaseBoardManufacturer is not NULL;

	insert into #XMLHardwareHelpTbl                
	select  NULL,  'MOTHERBOARD',   BaseBoardProduct + BaseBoardManufacturer, '', '', NULL, NULL, NULL, NULL, NULL, NULL,
	        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
	from #XMLSystemInfoOldBIOSTbl;

	 
			
	---------------------------  BIOS  --------------------------------

	select *  into #XMLSystemInfoBIOSTbl 
	from openxml(@XML, '/ResearchInfo/SystemInfo/Hardware/Motherboard/BIOS', 1)
	with (  Vendor  nvarchar(500),
			Version  nvarchar(500),
			ReleaseDate nvarchar(100), 
			ROMsize int); 

    Update #XMLSystemInfoBIOSTbl
    set ROMsize = NULL where ROMsize = 0;
     
    insert into #XMLHardwareHelpTbl                
	select  NULL,  'BIOS', Vendor, '', '' , ROMsize, NULL, NULL, NULL, NULL, NULL,
	        Version, NULL, NULL, NULL, NULL, ReleaseDate, NULL, NULL, NULL, NULL, NULL
	from #XMLSystemInfoBIOSTbl;


	---------------------------  SystemSlot  --------------------------------

	select *  into #XMLSystemInfoSystemSlotTbl 
	from openxml(@XML, '/ResearchInfo/SystemInfo/Hardware/Motherboard/SystemSlot', 1)
	with (  Type  nvarchar(500),
			BusWidth  nvarchar(500),
			Length nvarchar(500),
			CurrentUsage nvarchar(100)); 

    insert into #XMLHardwareHelpTbl                
	select  NULL,  'SystemSlot', Type, '', '' , NULL, NULL, NULL, NULL, NULL, NULL,
	        NULL, NULL, NULL, BusWidth, Length, NULL, CurrentUsage, NULL, NULL, NULL, NULL
	from #XMLSystemInfoSystemSlotTbl;

    ----------------  Chipset  ----------------------------------------------------------------------

              
    select Vendor, ChipsetName, ChipsetCodename  into #ChipsetTbl
    from #XMLHardwareHelpTbl, ChipsetInfo
    where Class = 'SYSTEM'
    and (DeviceDesc LIKE  Vendor + '%' or DeviceDesc LIKE  '% '+ Vendor + '%')
    and (    (ChipsetName is not null and len(ChipsetName) <> 0 and DeviceDesc LIKE '%[ \/]' + ChipsetName + '%') 
          or ( Northbridge is not null and len(Northbridge) <> 0 and (DeviceDesc LIKE '%[ \/]' + Northbridge + '%'))
        )


    select distinct Vendor into #ChipsetVendorTbl
    from #ChipsetTbl;

    select distinct ChipsetName into #ChipsetNameTbl
    from #ChipsetTbl where ChipsetName <> '';

    select distinct ChipsetCodename into #ChipsetCodenameTbl 
    from #ChipsetTbl where ChipsetCodename <> '';


    select @VendorCount = count(*) from #ChipsetVendorTbl;

    if (@VendorCount <> 1)
        set @ChipsetDesc = '';
    else
    begin
         
            select @ChipsetDesc = Vendor from #ChipsetVendorTbl;

           -----  Name ------

           set @ChipsetNameList = '';

		   while exists (select top(1) * from #ChipsetNameTbl)
		   begin
					select @ChipsetName =  ChipsetName from #ChipsetNameTbl;
                    set @ChipsetNameList = @ChipsetNameList + @ChipsetName + '/' ;

					delete from #ChipsetNameTbl where ChipsetName = @ChipsetName;
		   end

           set @ChipsetNameList = RTRIM(@ChipsetNameList);

		   if ( LEN(@ChipsetNameList) >0 )
				 set @ChipsetNameList = LEFT(@ChipsetNameList,  LEN(@ChipsetNameList) - 1);

           if (@ChipsetNameList is not NULL)
               set @ChipsetDesc = @ChipsetDesc + ' ' + @ChipsetNameList;


            -----  Codename ------

            set @ChipsetCodenameList = '';

			while exists (select top(1) * from #ChipsetCodenameTbl)
			begin
					select @ChipsetCodename =  ChipsetCodename from #ChipsetCodenameTbl;
                    set @ChipsetCodenameList = @ChipsetCodenameList + @ChipsetCodename + ', ' ;

					delete from #ChipsetCodenameTbl where ChipsetCodename = @ChipsetCodename;
			end

            set @ChipsetCodenameList = RTRIM(@ChipsetCodenameList);

			if ( LEN(@ChipsetCodenameList) >0 )
				  set @ChipsetCodenameList = LEFT(@ChipsetCodenameList,  LEN(@ChipsetCodenameList) - 1);

            if (@ChipsetCodenameList is not NULL)
                set @ChipsetDesc = @ChipsetDesc + ' ' + @ChipsetCodenameList;

            
    end 


	---------------------------  Baseboard  --------------------------------

	select *  into #XMLSystemInfoBaseboardTbl 
	from openxml(@XML, '/ResearchInfo/SystemInfo/Hardware/Motherboard/Baseboard', 1)
	with (  Manufacturer  nvarchar(500),
			Product  nvarchar(500),
			Version nvarchar(500),
			SerialNumber nvarchar(500)); 

	select *  into #XMLSystemInfoSystemInformationTbl 
	from openxml(@XML, '/ResearchInfo/SystemInfo/Hardware/Motherboard/SystemInformation', 1)
	with (  SystemUUID uniqueidentifier,
			WakeUpType nvarchar(500)); 
			
	select *  into #XMLSystemInfoProcessorUpgradeTbl 
	from openxml(@XML, '/ResearchInfo/SystemInfo/Hardware/Motherboard/ProcessorInformation', 1)
	with (  ProcessorUpgrade nvarchar(500));    


    select @SystemUUID = SystemUUID, @WakeUpType = WakeUpType 
	from #XMLSystemInfoSystemInformationTbl;

	select @Socket = ProcessorUpgrade
	from #XMLSystemInfoProcessorUpgradeTbl;

    if (@SystemUUID = '00000000-0000-0000-0000-000000000000')
       set @SystemUUID = NULL;

    select @N = count(*) from #XMLSystemInfoBaseboardTbl;
    if (@N > 0)
    begin
            delete from  #XMLHardwareHelpTbl where Class = 'MOTHERBOARD';

		    Update #XMLSystemInfoBaseboardTbl
			set Manufacturer = ' (' + Manufacturer + ')'
			where Manufacturer <> '' and Manufacturer is not NULL;

			insert into #XMLHardwareHelpTbl                
			select  NULL,  'MOTHERBOARD', Product + Manufacturer, '', Manufacturer , NULL, NULL, NULL, NULL, NULL, SerialNumber,
					Version,  @SystemUUID, @WakeUpType, NULL, NULL, NULL, NULL, NULL, @Socket, NULL, @ChipsetDesc
			from #XMLSystemInfoBaseboardTbl;
          
    end
    else if exists(select top(1) * from #XMLHardwareHelpTbl where class = 'MOTHERBOARD')
	begin
			update #XMLHardwareHelpTbl               
			set   UniversalUniqueID = @SystemUUID, WakeUpType = @WakeUpType, Socket = @Socket, Chipset = @ChipsetDesc
			where Class = 'MOTHERBOARD';
	end
	else if (    ( @SystemUUID <> '' and  @SystemUUID is not null )
	          or ( @WakeUpType <> '' and  @WakeUpType is not null )
			  or ( @Socket <> ''     and  @Socket is not null ) 
              or ( @ChipsetDesc <> ''     and  @ChipsetDesc is not null ) )
	begin
			insert into #XMLHardwareHelpTbl                
			values  (NULL,  'MOTHERBOARD', '', '', '' , NULL, NULL, NULL, NULL, NULL, NULL,
					NULL,  @SystemUUID, @WakeUpType, NULL, NULL, NULL, NULL, NULL, @Socket, NULL, @ChipsetDesc);
	end
	
	
	--------------------------- PROCESSOR  -------------------------------- 

	-- remain only 1 processor and fill field Quantity with its quantity

	set  @ProcessorName = '';  
	set  @Quantity = 0;

	select @ProcessorName = DeviceDesc from #XMLHardwareHelpTbl 
	where  Class =   'PROCESSOR'; 

	select @Quantity = Count(*) from #XMLHardwareHelpTbl 
	where  Class =   'PROCESSOR'; 

	set @Frequency = dbo.GetFrequencyFromProcessorName(@ProcessorName); 

	--  deleting multiple strings for processor and puting one with Quantity
	delete from  #XMLHardwareHelpTbl 
	where  Class =   'PROCESSOR'; 

	insert into #XMLHardwareHelpTbl  
	values (NULL,  'PROCESSOR', @ProcessorName, '', '' , NULL, NULL, @Frequency, @Quantity, NULL, NULL,
	        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);



    --------------  NetworkAdapter   --------------------------------------------------------------

    select *  into #XMLNetworkAdapterTbl 
	from openxml(@XML, '/ResearchInfo/SystemInfo/Hardware/NetworkAdapter', 1)
	with (   Description  nvarchar(450),
             MAC nvarchar(450));    

    select @Count = Count(*) from #XMLNetworkAdapterTbl;
    if (@Count > 0)
    begin
                delete from #XMLHardwareHelpTbl  
                where  Class =   'NET'; 

                insert into #XMLHardwareHelpTbl 
                select NULL,   'NET', Description, '', '', NULL, NULL, NULL, NULL, NULL, NULL,
				        NULL, NULL, NULL, NULL, NULL, NULL, NULL, MAC, NULL, NULL, NULL
                from #XMLNetworkAdapterTbl;        
    end 

  
	-------------------------   parameter from agent  ---------------------------------------------------

	 if (@InspectionType = 1 or @InspectionType = 2)
	 begin 

	   ---------------------- Memory  ------------------------------------------------------------  

	   CREATE TABLE #XMLSystemInfoMemoryTbl(
	  [ID] [int] IDENTITY(1,1) NOT NULL,
	  [Capacity] [int]  NULL,
	  [FormFactor] [nvarchar](500) collate Cyrillic_General_CI_AS  NULL,
	  [MemoryType] [nvarchar](500) collate Cyrillic_General_CI_AS  NULL,
		 ) ON [PRIMARY]; 

		insert  into #XMLSystemInfoMemoryTbl 
		select *  
		from openxml(@XML, '/ResearchInfo/SystemInfo/Hardware/Memory', 1)
		with (  Capacity nvarchar(100),
				FormFactor nvarchar(100),
				MemoryType nvarchar(100));
				   -- Speed  int);       --- frequency       -- показываем только интегральную характеристику, строку с перечислением Capacity

	              
		select @Quantity = Count(*) from #XMLSystemInfoMemoryTbl;

		set @I = 1;
		set @MemoryDesc = '';
		set @Memory = 0;

		while (@I <= @Quantity)
		begin
				   select @Capacity = Capacity from #XMLSystemInfoMemoryTbl
				   where ID = @I;

				   set @Memory =@Memory + @Capacity;                                  --  adding number
				  set @MemoryDesc = @MemoryDesc + @Capacity  + ', ' ;         -- adding string

				   set @I = @I + 1;           
		end

		-------------  deleting last  ', '  -----------------------------
		set @MemoryDesc = RTRIM(@MemoryDesc);
		set @L = LEN(@MemoryDesc); 
		if (@L >0)
		begin 
			  set @MemoryDesc = LEFT(@MemoryDesc, @L - 1);
		end

		if ( @MemoryDesc <> '')
		begin 

				set @MemoryDesc = '(' + @MemoryDesc + ')';

				/*update  #XMLHardwareHelpTbl                
				set Description = @Description, Quantity = @Quantity
				where class =   'MEMORY'; */

				select @FormFactor = FormFactor, @MemoryType = MemoryType from #XMLSystemInfoMemoryTbl;

				if ( (@MemoryType <> '')   and  (@MemoryType is not NULL)  and (@MemoryType <> 'Unknown') )
				set @MemoryDesc = @MemoryDesc + ', ' + @MemoryType;
		        
				select @MemoryDeviceDesc = DeviceDesc from #XMLHardwareHelpTbl  
				where  Class =   'MEMORY'; 
		        
				delete from #XMLHardwareHelpTbl  
				where  Class =   'MEMORY';                      

				insert into #XMLHardwareHelpTbl   values (NULL,  'MEMORY', @MemoryDeviceDesc +  ' ' + @MemoryDesc, '', '' , @Memory, NULL, NULL, @Quantity, NULL, NULL,
				                                          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL); 
		 end

		---------------  CentralProcessor  -------------------------------------------------------

		select *  into #XMLSystemInfoCentralProcessorTbl 
		from openxml(@XML, '/ResearchInfo/SystemInfo/Hardware/CentralProcessor', 1)
		with (  NumberOfCores int,
				MaxClockSpeed  int);       --- frequency

		set @Quantity = 0;
		set @Frequency = 0;

		select @Quantity = NumberOfCores, @Frequency =  MaxClockSpeed
		from #XMLSystemInfoCentralProcessorTbl ;

		if(@Quantity = 0 or @Quantity is NULL) 
		begin
				select @Quantity = Count(*) from #XMLSystemInfoCentralProcessorTbl;                                      
		end 

		if (@Quantity > 0)
		begin
				  update #XMLHardwareHelpTbl 
				  set Quantity = @Quantity
				  where class = 'PROCESSOR';
		end 

		if (@Frequency > 0)
		begin
				  update #XMLHardwareHelpTbl 
				  set Frequency = @Frequency
				  where class = 'PROCESSOR';
		end 


		----------------  Disc  ----------------------------------------------------------------------

		select *  into #XMLSystemInfoDiskTbl 
		from openxml(@XML, '/ResearchInfo/SystemInfo/Hardware/Disk', 1)
		with (  Caption nvarchar(1000),
				  Size bigint,
				  FreeSpace  bigint,
				  SerialNumber nvarchar(1000));  

		select @Count = Count(*) from #XMLSystemInfoDiskTbl;
		if (@Count > 0)
		begin
				  delete from #XMLHardwareHelpTbl  
				  where  Class =  'DISKDRIVE'; 

				  update #XMLSystemInfoDiskTbl
                  set Size = NULL where Size = 0;

                  update #XMLSystemInfoDiskTbl
                  set FreeSpace = NULL where FreeSpace = 0;

				   insert into #XMLHardwareHelpTbl 
				   select NULL,  'DISKDRIVE', Caption, '', '', Size, FreeSpace, NULL, NULL, NULL, SerialNumber,
				          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
				   from #XMLSystemInfoDiskTbl;        
		end

          --------------  VideoController   --------------------------------------------------------------

        select *  into #XMLSystemInfoVideoControllerTbl 
		from openxml(@XML, '/ResearchInfo/SystemInfo/Hardware/VideoController', 1)
		with (   Name  nvarchar(450),
                    AdapterRAM int);    

        select @Count = Count(*) from #XMLSystemInfoVideoControllerTbl;
        if (@Count > 0)
        begin
                  delete from #XMLHardwareHelpTbl  
                  where  Class =   'DISPLAY'; 

                   insert into #XMLHardwareHelpTbl 
                   select NULL,   'DISPLAY', Name, '', '', AdapterRAM, NULL, NULL, NULL, NULL, NULL,
				          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
                   from #XMLSystemInfoVideoControllerTbl;        
        end 

end

                -----------------------------------------------------------------------------------------------------
 
                select @HardwareAmount = Count(*) from #XMLHardwareHelpTbl;
--              We don't need to raise error in case of no hardware (E.g. importing old xml without hardware info)
--                 if (@HardwareAmount<1 or @HardwareAmount is null)
--                 begin
--                     RAISERROR ('Installed hardware list is empty', 12, 1);
--                 end
--                 else
                 if (@HardwareAmount >= 1)
                 begin  

                       select @DefaultUnitName = DefaultUnitName from ObjectRoot;
                       select @DefaultManufacturer = DefaultManufacturer from ObjectRoot;
                       select @DefaultClass = DefaultClass from ObjectRoot;

                       update #XMLHardwareHelpTbl
                       set DeviceDesc = @DefaultUnitName
                       where DeviceDesc is NULL or DeviceDesc= '';

                       update #XMLHardwareHelpTbl
                       set Mfg = @DefaultManufacturer
                       where Mfg is NULL or Mfg= '';

                       update #XMLHardwareHelpTbl
                       set Class = @DefaultClass
                       where Class is NULL or Class= '';
                       
                       update #XMLHardwareHelpTbl
                       set DeviceDesc = dbo.GetNameFromTemplate(DeviceDesc, 'DeviceDesc');
                       
                       update #XMLHardwareHelpTbl
                       set DeviceDesc = REPLACE(DeviceDesc, ';', ','), Class = REPLACE(Class, ';', ','),
					       Version = REPLACE(Version, ';', ','), Chipset = REPLACE(Chipset, ';', ','),
						   Service = REPLACE(Service, ';', ','), Socket = REPLACE(Socket, ';', ','),
						   BusLength = REPLACE(BusLength, ';', ','), BusWidth = REPLACE(BusWidth, ';', ',');                      
					   
					   update #XMLHardwareHelpTbl
					   set DeviceDesc = ltrim(rtrim(DeviceDesc)), Class = ltrim(rtrim(Class)),
					       Version = ltrim(rtrim(Version)), Chipset = ltrim(rtrim(Chipset)),
						   Service = ltrim(rtrim(Service)), Socket = ltrim(rtrim(Socket)),
						   BusLength = ltrim(rtrim(BusLength)), BusWidth = ltrim(rtrim(BusWidth));   						   

                       select @HardwareContainerGUID = ObjectGUID from ObjectRelations where ObjectType = 'HardwareContainer';


                       -- --------  searching hardware in Hardware and adding  if absent -----------------------
                               
                       select @MaxHardwareID = max(ID) from #XMLHardwareHelpTbl;

                       set @I = 1;
        
                       while (@I <= @MaxHardwareID)
                       begin

                              if exists(select top(1) * from #XMLHardwareHelpTbl where ID = @I)
                              begin
										   set @HardwareGUID = NULL; 

										   select @HardwareGUID =  H.ObjectGUID  from Hardware H, #XMLHardwareHelpTbl XH
										   where  XH.Class = H.Class  and XH.DeviceDesc = H.DeviceDesc  
													and ( (XH.Memory = H.Memory)            or (XH.Memory is NULL       and  H.Memory is NULL) )
													and ( (XH.Frequency = H.Frequency)      or (XH.Frequency is NULL    and  H.Frequency is NULL) )
													and ( (XH.Quantity = H.Quantity)        or (XH.Quantity is NULL     and  H.Quantity is NULL) )													  
													and ( (XH.Version = H.Version)          or (XH.Version is NULL      and  H.Version is NULL) )
													and ( (XH.WakeupType = H.WakeupType)    or (XH.WakeupType is NULL   and  H.WakeupType is NULL) ) 
													and ( (XH.BusWidth = H.BusWidth)        or (XH.BusWidth is NULL     and  H.BusWidth is NULL) )
													and ( (XH.BusLength = H.BusLength)      or (XH.BusLength is NULL    and  H.BusLength is NULL) )
													and ( (XH.Socket = H.Socket)            or (XH.Socket is NULL       and  H.Socket is NULL) )  
													and ( (XH.Service = H.Socket)           or (XH.Service is NULL      and  H.Service is NULL) ) 
                                                    and ( (XH.Chipset = H.Chipset)          or (XH.Chipset is NULL      and  H.Chipset is NULL) )    
													/*and XH.Speed = H.Speed */   and XH.ID = @I;
			                      
											if (@HardwareGUID is NULL)
											begin
			                
												   set @HardwareGUID = NEWID();
			                                       
												   update #XMLHardwareHelpTbl
												   set ObjectGUID = @HardwareGUID
												   where ID = @I;                    

												   insert into dbo.Hardware
												   select ObjectGUID, @HardwareContainerGUID, Class, DeviceDesc, Memory,  Frequency, Quantity, Speed, 1,
												          Version, WakeupType, BusWidth, BusLength, Socket, Service, Chipset
												   from #XMLHardwareHelpTbl
												   where ID = @I;
												   if (@@Error <> 0)   set @Check = -1; 
			                     
											end      

											-- adding in HardInstallation

											select @CharReleaseDate = ReleaseDate
											from #XMLHardwareHelpTbl
											where ID = @I; 
					
											if(isdate(@CharReleaseDate) = 1)
												set @ReleaseDate = @CharReleaseDate;
											else
												set @ReleaseDate = NULL; 
														                                
											insert into dbo.HardInstallation 
											select NEWID(), @HardwareGUID, @InspectionGUID, @AddingDate, Mfg, FreeMemory,
											       SerialNumber, UniversalUniqueID, CurrentUsage, @ReleaseDate, MAC
											from #XMLHardwareHelpTbl
											where ID = @I; 
											if (@@Error <> 0)   set @Check = -1;   
                                 end           
                                      
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
    Set @Message2 = 'Ошибка добавления нового оборудования в БД: ';
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