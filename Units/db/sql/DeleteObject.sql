CREATE PROCEDURE [dbo].[DeleteObject] 
(@ObjectGUID nvarchar(50), @ObjectType nvarchar(350))

AS
BEGIN
	
	SET NOCOUNT ON;

BEGIN TRY  

SET DATEFORMAT ymd;

BEGIN TRANSACTION

declare @String  nvarchar(3000);
declare @Message  nvarchar(max);
declare @Check int;
declare @Count int;
declare @DefaultCategoryGUID uniqueidentifier;
declare @ComputerName  nvarchar(450);

declare @InspectionList   nvarchar(max);

declare @TabInspection Table(ID int IDENTITY(1,1), 
                             ComputerGUID uniqueidentifier,
                             ComputerName nvarchar(450) collate Cyrillic_General_CI_AS );
declare @I int;
declare @L int;

set @Check = 0;

if (@ObjectType = 'Task')
begin
	-- если удаляем Task, то сначала удалить ComputerLink-и,
	delete from ComputerLink where ParentGuid = @ObjectGUID;
    if (@@Error <> 0)   set @Check = -1;

	-- а потом сам таск 
	delete from ObjectRelations where ObjectGUID = @ObjectGUID;	 
    if (@@Error <> 0)   set @Check = -1;

end
else if (@ObjectType = 'Passport')
begin
         -- удаляем линки паспорта с правилами лицензирования софта
             delete from LicenseRuleLink where ParentGuid = @ObjectGUID;
             if (@@Error <> 0)   set @Check = -1;

         -- удаляем линки паспорта с правилами лицензирования железа
             delete from HardwareLicenseRuleLink where ParentGuid = @ObjectGUID;
             if (@@Error <> 0)   set @Check = -1;
 
         -- удаляем линки паспорта с компьютерами
             delete from PassportLink
             where  PassportGUID = @ObjectGUID;
             if (@@Error <> 0)   set @Check = -1;

         -- удаляем паспорт
             delete from Passport where ObjectGUID = @ObjectGUID;
             if (@@Error <> 0)   set @Check = -1;
         
end
else if (@ObjectType = 'Computer')
begin
        -- обнуляем поле в Task, отвечающее за синхронизацию данных с АД
		   update Task
		   set SNChanged = NULL
		   where ObjectGUID in (select ParentGuid from ComputerLink
		                        where ComputerGUID = @ObjectGUID);
		   if (@@Error <> 0)   set @Check = -1;


        -- удаляем линки компьютера с заданиями
             delete from ComputerLink where ComputerGUID = @ObjectGUID;
             if (@@Error <> 0)   set @Check = -1;

         -- удаляем компьютер
             delete from Computer where ObjectGUID = @ObjectGUID;
             if (@@Error <> 0)   set @Check = -1;
         
end
else if (@ObjectType like 'LicenseRule' )
begin

	     -- удаляем линки правила с паспортами
             delete from LicenseRuleLink where LicenseRuleGUID = @ObjectGUID;
             if (@@Error <> 0)   set @Check = -1; 

         -- удаляем правило
             delete from LicenseRule where ObjectGUID = @ObjectGUID;
             if (@@Error <> 0)   set @Check = -1; 
	    
end
else if (@ObjectType like 'HardwareLicenseRule' )
begin

	     -- удаляем линки правила с паспортами
             delete from HardwareLicenseRuleLink where HardwareLicenseRuleGUID = @ObjectGUID;
             if (@@Error <> 0)   set @Check = -1; 

         -- удаляем правило
             delete from HardwareLicenseRule where ObjectGUID = @ObjectGUID;
             if (@@Error <> 0)   set @Check = -1; 
	    
end
else if (@ObjectType = 'Category')
begin
        -- проверка, что не удаляется категория безопасности
        set @Count = 0;
 
        select @Count = Count(*) from SecurityTypeLU S, Category C
        where S.SecurityType = C.Name and C.ObjectGUID = @ObjectGUID;

        if (@Count > 0)
        begin
                Set @Message = 'Указанную категорию удалить нельзя. Она используется в отчетах по безопасности компьютеров';
                RAISERROR(@Message, 12, 1);
			    Return -1;
        end
        else
        begin   
				update SoftType
				set CategoryGUID = NULL where CategoryGUID = @ObjectGUID;
				if (@@Error <> 0)   set @Check = -1;
		        
				delete from Category where ObjectGUID = @ObjectGUID;
				if (@@Error <> 0)   set @Check = -1;    
        end 
	    
end
else if (@ObjectType in ('LibProductFile', 'LicenseImage', 'Synonym', 'Subdivision', 'ProhibitionRule') or @ObjectType like '%Link')
begin

		set @String = 'delete from ' + @ObjectType + ' where ObjectGUID = '  + '''' + @ObjectGUID  + '''';
		execute(@String);
		if (@@Error <> 0) set @Check = -1;
	
end	
else if (@ObjectType = 'SoftType')
begin
        -- check for existing such soft in computer inspection data
        
        if exists(select top(1) * from Soft S, Installation I
                  where S.SoftTypeGUID = @ObjectGUID and S.ObjectGUID = I.SoftGUID)
        begin

				insert into @TabInspection
				SELECT distinct A.ParentGUID, NULL  
				FROM     Action A, Installation I, Soft S
				WHERE  S.SoftTypeGUID = @ObjectGUID and I.SoftGUID = S.ObjectGUID 
				       and I.ParentGUID = A.ObjectGUID;
				       
				update  @TabInspection
				set  ComputerName = C.Name
				from Computer C, @TabInspection TI
				where C.ObjectGUID = TI.ComputerGUID;     

				select @Count = Count(*) from @TabInspection;

				set @I = 1;
				set @InspectionList = '';

				while (@I < = @Count)
				begin
						   select @ComputerName = ComputerName from @TabInspection where ID = @I;
						   set @InspectionList = @InspectionList + @ComputerName + '; ' ;
						   set @I = @I + 1; 
				end

			   -------------  deleting last  '; '  -----------------------------
			   set @L = LEN(@InspectionList); 
			   if (@L >2)
               begin
				  set @InspectionList = LEFT(@InspectionList, @L - 1);
                  Set @Message = 'На указанный продукт ссылаются данные инспектирования компьютеров. Чтобы его удалить, необходимо вначале удалить данные инспектирования компьютеров: ' + @InspectionList;
               end
               else
                  Set @Message = 'На указанный продукт ссылаются данные инспектирования компьютеров. Чтобы его удалить, необходимо вначале удалить данные инспектирования';

             RAISERROR(@Message, 12, 1);
			 Return -1;
		end 
		else
		begin
		     delete from SoftType where ObjectGUID = @ObjectGUID;
		     if (@@Error <> 0) set @Check = -1;
		end          
	
end	
else if (@ObjectType = 'Publisher')
begin
        -- check for existing such soft in computer inspection data
        
        if exists(select top(1) * from SoftType ST, Soft S, Installation I
                  where S.SoftTypeGUID = ST.ObjectGUID and S.ObjectGUID = I.SoftGUID
                        and ST.PublisherGUID = @ObjectGUID)
        begin

				insert into @TabInspection
				SELECT distinct A.ParentGUID, NULL
				FROM     Action A, Installation I, Soft S, SoftType ST
				WHERE  ST.PublisherGUID = @ObjectGUID and S.SoftTypeGUID = ST.ObjectGUID 
				       and I.SoftGUID = S.ObjectGUID  and I.ParentGUID = A.ObjectGUID;
				       
				update  @TabInspection
				set  ComputerName = C.Name
				from Computer C, @TabInspection TI
				where C.ObjectGUID = TI.ComputerGUID;     

				select @Count = Count(*) from @TabInspection;

				set @I = 1;
				set @InspectionList = '';

				while (@I < = @Count)
				begin
						   select @ComputerName = ComputerName from @TabInspection where ID = @I;
                           set @InspectionList = @InspectionList + @ComputerName+ '; ' ;
						   set @I = @I + 1; 
				end

			   -------------  deleting last  '; '  -----------------------------
			   set @L = LEN(@InspectionList); 
			   if (@L >2)
               begin
				  set @InspectionList = LEFT(@InspectionList, @L - 1);
                  Set @Message = 'На указанный продукт ссылаются данные инспектирования компьютеров. Чтобы его удалить, необходимо вначале удалить данные инспектирования компьютеров: ' + @InspectionList;
               end
               else
                  Set @Message = 'На указанный продукт ссылаются данные инспектирования компьютеров. Чтобы его удалить, необходимо вначале удалить данные инспектирования';


			 RAISERROR(@Message, 12, 1);
			 Return -1;
		end 
		else
		begin
		     
		     delete from Publisher
		     where ObjectGUID = @ObjectGUID;  
		     if (@@Error <> 0) set @Check = -1;                     
		
		end          
	
end	
else 
begin
		Set @Message = 'Объекты c типом ' + @ObjectType + ' удалять запрещено';
		RAISERROR(@Message, 12, 1);
		Return -1;		
end  
 

if (@Check = 0)
begin
      COMMIT TRANSACTION    
end   
else 
begin 
      Set @Message = 'Транзакция отменена';
      RAISERROR(@Message, 12, 1);
end

END TRY
BEGIN CATCH

           if (Error_Message()  is NULL or Error_Message() = '')
			   set @Message = 'Ошибка БД при удалении объекта';
		   else
			   set @Message = Error_Message();
   
           ROLLBACK TRANSACTION
           RAISERROR(@Message, 12, 1);
           Return -1;

 END CATCH
	
END