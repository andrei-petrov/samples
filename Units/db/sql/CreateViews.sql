CREATE PROCEDURE [dbo].[CreateViews] 
--  creates views for objects that where are not in ObjectRelations

AS
BEGIN

SET NOCOUNT ON;
	
declare @ForExecute     nvarchar(1000);
declare @Message     nvarchar(1000);
declare @Object      nvarchar(50);
declare @I int;

BEGIN TRY

      declare @TabObjects Table(ID int IDENTITY(1,1), Object nvarchar(50) collate Cyrillic_General_CI_AS );


      insert into @TabObjects values('FilesFound');
      insert into @TabObjects values('Action');
      insert into @TabObjects values('Publisher');
      insert into @TabObjects values('Category');
      insert into @TabObjects values('Passport');
      insert into @TabObjects values('PassportLink');
      insert into @TabObjects values('HardwareLicenseRuleLink');
      --insert into @TabObjects values('AttrSchema');
      --insert into @TabObjects values('Task');
      insert into @TabObjects values('LicenseRuleLink');
      insert into @TabObjects values('ComputerLink');
      --insert into @TabObjects values('Report');
      insert into @TabObjects values('Hardware');
      insert into @TabObjects values('Subdivision');
      insert into @TabObjects values('ObjectSchema');
      insert into @TabObjects values('LicenseImage');
      insert into @TabObjects values('USBDevice');
      
      set @I = 1;
      
      while (@I <= 14)
      begin

            select @Object = Object from   @TabObjects where ID = @I;
            set @ForExecute =  'CREATE VIEW View' + @Object +  
                               ' AS select ' + '''' +  @Object + '''' + ' as ObjectType, Obj.* from  '
                                +  @Object + ' Obj';

            execute (@ForExecute);

            set @I = @I + 1;
      end 

      
           
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при создании представлений';
	else
	   set @Message = Error_Message();
   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
