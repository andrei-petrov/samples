CREATE PROCEDURE [dbo].[GetDataFromOldDB] 
(@OldDB nvarchar(300), @MigrationPart nvarchar(100), @ObjectGUIDList nvarchar(max) = NULL)

AS
BEGIN

SET NOCOUNT ON;

SET DATEFORMAT ymd;
	
declare @Message  nvarchar(max);
declare @ForExec  nvarchar(max);
declare @OldTable nvarchar(450);
declare @OldObjectRel nvarchar(450);

declare @ParentContainerGUID nvarchar(450);
declare @AttrList nvarchar(max);
declare @NewAttrValuesList nvarchar(4000);
declare @ObjectType nvarchar(450);
declare @ParentContainer nvarchar(450);
declare @IsParentContainer bit;
declare @WithObjRel bit;
declare @M int;
declare @N int;
declare @I int;
declare @Check smallint;



BEGIN TRY

BEGIN TRANSACTION

set @Check = 0;

declare @TabObject Table(InsertOrder int, ObjectType nvarchar(450), IsParentContainer bit, AttrList nvarchar(max), 
                              NewAttrValuesList nvarchar(4000), WithObjRel bit);


--  ID - because of object order  is important
insert into @TabObject
select InsertOrder, ObjectType, IsParentContainer, AttrList, NewAttrValuesList, WithObjRel
from DBMigration
where MigrationPart = @MigrationPart;
if (@@Error <> 0)  
     RAISERROR('Ошибка загрузки метаданных', 12, 1);


set @OldDB = '[' + @OldDB + ']';

set @OldObjectRel = @OldDB + '.dbo.ObjectRelations';

select @M = min(InsertOrder) from @TabObject;
select @N = max(InsertOrder) from @TabObject;
set @I = @M;


while(@I <= @N)
begin
        
			select @ObjectType = ObjectType, @IsParentContainer = IsParentContainer, @AttrList = AttrList, 
				   @NewAttrValuesList = NewAttrValuesList, @WithObjRel = WithObjRel
			from @TabObject where InsertOrder = @I;

			set @OldTable = @OldDB + '.dbo.' + @ObjectType;

--  Если парент является контейнером, гуид контейнера берем из новой БД

			if (@IsParentContainer = 1)
			begin
				   set @ParentContainer = @ObjectType + 'Container';

				   select @ParentContainerGUID = ObjectGUID 
				   from ObjectRelations
				   where ObjectType = @ParentContainer;

				   set @AttrList = '''' + @ParentContainerGUID + '''' + ', ' + @AttrList;
			end

--  Новые атрибуты, отсутствующие в старой таблице

			if ( @NewAttrValuesList <> '')
				 set @AttrList = @AttrList + ', ' + @NewAttrValuesList;

--  Если данные объекта хранились в таблице данного объекта и в ObjectRelations 

			if (@WithObjRel = 1)
				  set @OldTable  =  '(select  Rel.Name, Rel.ParentGUID, Obj.* ' + 
								    ' from ' + '' + @OldObjectRel  + '' + ' Rel, '  + '' +  @OldTable  + '' +  ' Obj'  + 
								    ' where Obj.ObjectGUID = Rel.ObjectGUID) S';



            set @ForExec  =  'insert into '          + @ObjectType +
							 ' select ObjectGUID,  '  + @AttrList +
							 ' from '                 + @OldTable;


			if (@ObjectType = 'Action' and @ObjectGUIDList <> '' and @ObjectGUIDList is not NULL)
                   set @ForExec  =  @ForExec  + ' where ObjectGUID IN (' + @ObjectGUIDList + ')';


            if (@ObjectType in ('Installation', 'HardInstallation', 'FileInstallation', 'FilesFound')  
                            and @ObjectGUIDList <> '' and @ObjectGUIDList is not NULL)
                   set @ForExec  =   @ForExec + ' where ParentGUID IN (' + @ObjectGUIDList + ')';

	        
			execute(@ForExec);
			if (@@Error <> 0)  set @Check = -1;
                
        
		set @I = @I + 1;
           
end

if (@Check = 0)
        COMMIT TRANSACTION
else 
begin 
    RAISERROR('Транзакция отменена', 12, 1);
end


           
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка переноса данных в новую БД';
	else if (@ObjectType <> '' and @ObjectType is not null)
	   set @Message = 'Ошибка переноса данных в новую БД. ' + @ObjectType + ': '  + Error_Message();
    else
       set @Message = 'Ошибка переноса данных в новую БД. ' + Error_Message();

    ROLLBACK TRANSACTION 
   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
