use RIO_IPS
 
--DISABLE TRIGGER [trg_loggingDDLModify] ON DATABASE
 
-- очищаем БД от лишних пользователей
set nocount on
 
declare @sql nvarchar(1000)
declare user_cur cursor local
for
select name from sys.sysusers where issqlrole = 0 and status > 0
open user_cur
fetch next from user_cur into @sql
while @@FETCH_STATUS = 0
    begin
        set @sql  = 'drop user [' + @sql + ']'     
         
        begin try
            exec sp_executesql @sql            
        end try
 
        begin catch
            print error_message()
        end catch
         
 
        fetch next from user_cur into @sql
    end
close user_cur
deallocate user_cur
 
  
-- Права, пользователи и из зависимоости
declare @ID_LOGIN int= 0
 
truncate table DT_STRUCTURE_PDF_QUEVE DBCC CHECKIDENT ('DT_STRUCTURE_PDF_QUEVE', reseed, 1) WITH NO_INFOMSGS
 
 
truncate table CT_DOC_CARDS DBCC CHECKIDENT ('CT_DOC_CARDS', reseed, 1) WITH NO_INFOMSGS
delete from  CT_DOC_IMAGES DBCC CHECKIDENT ('CT_DOC_IMAGES', reseed, 1) WITH NO_INFOMSGS
truncate table ST_LOG_USERS DBCC CHECKIDENT ('ST_LOG_USERS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_OPEN_STAT DBCC CHECKIDENT ('DT_OPEN_STAT', reseed, 1) WITH NO_INFOMSGS
  
declare cur cursor local
    for
        select ID_LOGIN from ST_LOGINS where LOGIN_VALUE <> 'sa'
    open cur
    fetch next from cur into @ID_LOGIN
    while @@FETCH_STATUS = 0
        begin
  
            -- Удаляем пользователя из всех групп
            DELETE FROM SS_GROUP_MEMBERS WHERE ID_LOGIN = @ID_LOGIN
          
            -- Удаляем все права
            -- права на приложения
            DELETE FROM st_grant_application_user where ID_LOGIN = @ID_LOGIN
            -- права на процедуры
            DELETE FROM st_grant_procedure_user where ID_LOGIN = @ID_LOGIN
            -- права на отчёты Crystal report
            DELETE FROM st_grant_report_user where ID_LOGIN = @ID_LOGIN
            -- права на отчёты MS Office V1
            DELETE FROM st_grant_report_ole_user where ID_LOGIN = @ID_LOGIN
            -- права на отчёты MS Office V2
            DELETE FROM st_grant_report_office_user where ID_LOGIN = @ID_LOGIN
            -- права на фильтры к объектам
            DELETE FROM st_grant_object_filter_user where ID_LOGIN = @ID_LOGIN
            -- настройки полей объектов для пользователя
            DELETE FROM st_grant_object_fields_user where ID_LOGIN = @ID_LOGIN
            -- права на объекты
            DELETE FROM st_grant_object_user where ID_LOGIN = @ID_LOGIN
      
            delete from ST_SETTINGS_USERS where ID_LOGIN = @ID_LOGIN
  
            delete from ST_SETTINGS_OBJECTS where ID_LOGIN = @ID_LOGIN
  
            fetch next from cur into @ID_LOGIN
        end
delete from ST_LOGINS where LOGIN_VALUE <> 'sa'
  
  
-- очистка таблиц
truncate table DT_BOXES DBCC CHECKIDENT ('DT_BOXES', reseed, 1) WITH NO_INFOMSGS
delete from DT_PROJECTS_SETTINGS DBCC CHECKIDENT ('DT_PROJECTS_SETTINGS', reseed, 1) WITH NO_INFOMSGS
 
  
truncate table DT_CODES_2_LIST DBCC CHECKIDENT ('DT_CODES_2_LIST', reseed, 1) WITH NO_INFOMSGS
  
truncate table DT_CODES_LIST DBCC CHECKIDENT ('DT_CODES_LIST', reseed, 1) WITH NO_INFOMSGS
  
truncate table DT_PROJECT_FILES_ATTRIBUTES DBCC CHECKIDENT ('DT_PROJECT_FILES_ATTRIBUTES', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PAGE_FOLDERS DBCC CHECKIDENT ('DT_PAGE_FOLDERS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PROJECT_FILES DBCC CHECKIDENT ('DT_PROJECT_FILES', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PAGE_TYPE_CODES_MASKS DBCC CHECKIDENT ('DT_PAGE_TYPE_CODES_MASKS', reseed, 1) WITH NO_INFOMSGS
  
truncate table DT_PROJECT_FILES_ATTRIBUTES DBCC CHECKIDENT ('DT_PROJECT_FILES_ATTRIBUTES', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PROJECT_FILES_ATTRIBUTES DBCC CHECKIDENT ('DT_PROJECT_FILES_ATTRIBUTES', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PROJECT_FILES_ATTRIBUTES DBCC CHECKIDENT ('DT_PROJECT_FILES_ATTRIBUTES', reseed, 1) WITH NO_INFOMSGS
  
 
 
 
  
-- Группы документов и документы
truncate table DT_DOCUMENT_ATTRIBUTES DBCC CHECKIDENT ('DT_DOCUMENT_ATTRIBUTES', reseed, 1) WITH NO_INFOMSGS
DELETE FROM DT_PAGE_TYPES_GROUPS_PAGES DBCC CHECKIDENT ('DT_PAGE_TYPES_GROUPS_PAGES', reseed, 1) WITH NO_INFOMSGS
DELETE FROM DT_PAGE_TYPES_GROUPS WHERE PAGE_TYPE_GROUP_ID <> 2 DBCC CHECKIDENT ('DT_PAGE_TYPES_GROUPS', reseed, 3) WITH NO_INFOMSGS
  
declare @ID int = 0
set @id = 0
  
delete from DT_DOC_TYPES DBCC CHECKIDENT ('DT_DOC_TYPES', reseed, 1) WITH NO_INFOMSGS
insert into DT_DOC_TYPES (CODE, NAME, DESCRIPTION, DOC_FLAG, SHORT_NAME, PAGE_SAMPLE_FLAG, PAGE_PDF_FLAG)
values('0', 'Тест', 'Тест', 1, 'Тест', 1,1)
set @ID = SCOPE_IDENTITY()
update DT_DOC_TYPES with(rowlock) set CODE = cast(@id as char(4))  where DT_DOC_TYPES_ID = @id
INSERT INTO DT_DOCUMENT_ATTRIBUTES (ATTR_DOC_TYPE, ATTR_TYPE, ATTR_NAME, FROM_IMAGE, REQUIRED, IS_IDENTITY, ATTR_MASK, ATTR_AUTO_INDEX_FLAG)
values(@ID,4,'Номер досье', 1,1,0,'',3 )
  
  
  
INSERT INTO DT_PAGE_TYPES_GROUPS_PAGES (PAGE_TYPE_GROUP_ID, PAGE_TYPE_ID, BUTTON_NUMBER)
SELECT (2), @id, COALESCE(MAX(BUTTON_NUMBER), 0)+1 FROM DT_PAGE_TYPES_GROUPS_PAGES WHERE PAGE_TYPE_GROUP_ID=2
  
delete from DT_COMMON_DOCUMENT_ATTR DBCC CHECKIDENT ('DT_COMMON_DOCUMENT_ATTR', reseed, 1) WITH NO_INFOMSGS
insert into DT_COMMON_DOCUMENT_ATTR(ATTR_TYPE, ATTR_NAME) values (5, 'Номер документа')
  
delete from DT_PROJECT_PAGE_TYPES_GROUPS where PAGE_TYPE_GROUP_ID <> 2
delete from DT_PROJECT_PAGE_TYPES_GROUPS where PROJECT_ID <> 1
 
-- end block
  
-- Обнуление всех шаблонов
  
truncate table DT_PAGE_TYPE_SAMPLES_ATTRS DBCC CHECKIDENT ('DT_PAGE_TYPE_SAMPLES_ATTRS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PAGE_TYPE_SAMPLES_BLOCKS DBCC CHECKIDENT ('DT_PAGE_TYPE_SAMPLES_BLOCKS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PAGE_TYPE_SAMPLES_BLOCKS_KEYWORDS DBCC CHECKIDENT ('DT_PAGE_TYPE_SAMPLES_BLOCKS_KEYWORDS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PAGE_TYPE_SAMPLES_KEY_LINKS DBCC CHECKIDENT ('DT_PAGE_TYPE_SAMPLES_KEY_LINKS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PAGE_TYPE_SAMPLES_KEYWORDS DBCC CHECKIDENT ('DT_PAGE_TYPE_SAMPLES_KEYWORDS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PAGE_TYPE_SAMPLES DBCC CHECKIDENT ('DT_PAGE_TYPE_SAMPLES', reseed, 1) WITH NO_INFOMSGS
  
-- end block
  
truncate table DT_CONNECTION_LOGINS DBCC CHECKIDENT ('DT_CONNECTION_LOGINS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_COMPLECTS DBCC CHECKIDENT ('DT_COMPLECTS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_CONNECTION_SESSIONS DBCC CHECKIDENT ('DT_CONNECTION_SESSIONS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_CUSTOM_INPUT_FORMS DBCC CHECKIDENT ('DT_CUSTOM_INPUT_FORMS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_DDL_MODIFY_LOG DBCC CHECKIDENT ('DT_DDL_MODIFY_LOG', reseed, 1) WITH NO_INFOMSGS
truncate table DT_DOC_ATTR_ERR DBCC CHECKIDENT ('DT_DOC_ATTR_ERR', reseed, 1) WITH NO_INFOMSGS
truncate table DT_DOC_TYPES_ADDITIONAL
truncate table DT_EVENT_LOGS DBCC CHECKIDENT ('DT_EVENT_LOGS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_FAVOURITE_PAGE_TYPES DBCC CHECKIDENT ('DT_FAVOURITE_PAGE_TYPES', reseed, 1) WITH NO_INFOMSGS
  
-- Фильтры (по умолчанию)
truncate table DT_FILTERS DBCC CHECKIDENT ('DT_FILTERS', reseed, 1) WITH NO_INFOMSGS
  insert into DT_FILTERS(FILTER_NAME, FILTER_PROCEDURE_NAME, FILTER_TYPE, FILTER_MENU)
  values('Системная информация', 'projectfileID_sysinfo', 6,0)
  
truncate table DT_PAGE_TYPE_BUTTONS DBCC CHECKIDENT ('DT_PAGE_TYPE_BUTTONS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PAGE_TYPES_STAMPS DBCC CHECKIDENT ('DT_PAGE_TYPES_STAMPS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PARTS
truncate table DT_PDF_ATTRIBUTES DBCC CHECKIDENT ('DT_PDF_ATTRIBUTES', reseed, 1) WITH NO_INFOMSGS
delete from DT_PDF_NAME_FORMULAS where PDF_FORMULA_ID <> 1
truncate table DT_PDF_PATHS DBCC CHECKIDENT ('DT_PDF_PATHS', reseed, 1) WITH NO_INFOMSGS
insert into DT_PDF_PATHS(PDF_PATH) values ('\\localhost\server\Pdf')
truncate table DT_PROCEDURE_EXECUTION_LOG DBCC CHECKIDENT ('DT_PROCEDURE_EXECUTION_LOG', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PROCESSES DBCC CHECKIDENT ('DT_PROCESSES', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PROJECT_FILES_STATS DBCC CHECKIDENT ('DT_PROJECT_FILES_STATS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PROJECT_FILES_TAGS DBCC CHECKIDENT ('DT_PROJECT_FILES_TAGS', reseed, 1) WITH NO_INFOMSGS
delete from DT_PROJECTS where PROJECT_ID <> 1
truncate table DT_SIDE_BUTTONS DBCC CHECKIDENT ('DT_SIDE_BUTTONS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_STAMPS DBCC CHECKIDENT ('DT_STAMPS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_STRUCTURE DBCC CHECKIDENT ('DT_STRUCTURE', reseed, 1) WITH NO_INFOMSGS
truncate table DT_STRUCTURE_ADDITIONAL_ATTRIBUTES
truncate table DT_STRUCTURE_MESSAGES DBCC CHECKIDENT ('DT_STRUCTURE_MESSAGES', reseed, 1) WITH NO_INFOMSGS
truncate table DT_STRUCTURE_OPERATORS_LOG DBCC CHECKIDENT ('DT_STRUCTURE_OPERATORS_LOG', reseed, 1) WITH NO_INFOMSGS
truncate table SS_GROUPS_LOG DBCC CHECKIDENT ('SS_GROUPS_LOG', reseed, 1) WITH NO_INFOMSGS
truncate table ST_GBL_CONSTANTS_LOG DBCC CHECKIDENT ('ST_GBL_CONSTANTS_LOG', reseed, 1) WITH NO_INFOMSGS
truncate table ST_IO_LOGS DBCC CHECKIDENT ('ST_IO_LOGS', reseed, 1) WITH NO_INFOMSGS
truncate table ST_LOGINS_LOG DBCC CHECKIDENT ('ST_LOGINS_LOG', reseed, 1) WITH NO_INFOMSGS
truncate table DT_TASKS_CONTROL DBCC CHECKIDENT ('DT_TASKS_CONTROL', reseed, 1) WITH NO_INFOMSGS
truncate table DT_TASKS_PROCESSES_PROJECTS DBCC CHECKIDENT ('DT_TASKS_PROCESSES_PROJECTS', reseed, 1) WITH NO_INFOMSGS
truncate table XML_LOG DBCC CHECKIDENT ('XML_LOG', reseed, 1) WITH NO_INFOMSGS
truncate table DT_ATTRIBUTE_EVENT_LOGS DBCC CHECKIDENT ('DT_ATTRIBUTE_EVENT_LOGS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PROJECT_FILES_TABLES DBCC CHECKIDENT ('DT_PROJECT_FILES_TABLES', reseed, 1) WITH NO_INFOMSGS
truncate table DT_SET_COMPLECTS_LOG DBCC CHECKIDENT ('DT_SET_COMPLECTS_LOG', reseed, 1) WITH NO_INFOMSGS
truncate table DT_DOCUMENTS_TABLES DBCC CHECKIDENT ('DT_DOCUMENTS_TABLES', reseed, 1) WITH NO_INFOMSGS
truncate table DT_DOCUMENTS_TABLES_COLUMNS DBCC CHECKIDENT ('DT_DOCUMENTS_TABLES_COLUMNS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_DOCUMENTS_TABLES_COLUMNS_SUBSTS DBCC CHECKIDENT ('DT_DOCUMENTS_TABLES_COLUMNS_SUBSTS', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PAGE_TYPE_SAMPLES_ATTRS_HISTORY DBCC CHECKIDENT ('DT_PAGE_TYPE_SAMPLES_ATTRS_HISTORY', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PAGE_TYPE_SAMPLES_BLOCKS_HISTORY DBCC CHECKIDENT ('DT_PAGE_TYPE_SAMPLES_BLOCKS_HISTORY', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PAGE_TYPE_SAMPLES_BLOCKS_KEYWORDS_HISTORY DBCC CHECKIDENT ('DT_PAGE_TYPE_SAMPLES_BLOCKS_KEYWORDS_HISTORY', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PAGE_TYPE_SAMPLES_KEY_LINKS_HISTORY DBCC CHECKIDENT ('DT_PAGE_TYPE_SAMPLES_KEY_LINKS_HISTORY', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PAGE_TYPE_SAMPLES_KEYWORDS_HISTORY DBCC CHECKIDENT ('DT_PAGE_TYPE_SAMPLES_KEYWORDS_HISTORY', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PROJECT_FILES_WORDS_QUEVE DBCC CHECKIDENT ('DT_PROJECT_FILES_WORDS_QUEVE', reseed, 1) WITH NO_INFOMSGS
truncate table DT_INTERFACE_MESSAGES DBCC CHECKIDENT ('DT_INTERFACE_MESSAGES', reseed, 1) WITH NO_INFOMSGS
truncate table DT_INTERFACE_MESSAGES_LOG DBCC CHECKIDENT ('DT_INTERFACE_MESSAGES_LOG', reseed, 1) WITH NO_INFOMSGS
truncate table DT_CREATE_DOC_TYPES_LOG DBCC CHECKIDENT ('DT_CREATE_DOC_TYPES_LOG', reseed, 1) WITH NO_INFOMSGS
truncate table DT_PAGE_TYPE_SAMPLES_HISTORY DBCC CHECKIDENT ('DT_PAGE_TYPE_SAMPLES_HISTORY', reseed, 1) WITH NO_INFOMSGS
 
truncate table DIN_IndexingStatistic DBCC CHECKIDENT ('DIN_IndexingStatistic', reseed, 1) WITH NO_INFOMSGS
truncate table DIN_ScanStatistic DBCC CHECKIDENT ('DIN_ScanStatistic', reseed, 1) WITH NO_INFOMSGS
truncate table DIN_SeparatedStatistic DBCC CHECKIDENT ('DIN_SeparatedStatistic', reseed, 1) WITH NO_INFOMSGS
truncate table DIN_StructureStatistic DBCC CHECKIDENT ('DIN_StructureStatistic', reseed, 1) WITH NO_INFOMSGS
 
 
DBCC SHRINKDATABASE(N'RIO_IPS' )
DBCC SHRINKFILE (N'RIO_log' , 0, TRUNCATEONLY)
 
 
--declare @IPS_BK_NAME nvarchar(1000) = N'D:\SQL_BK\RIO_IPS_etalon_2_' + replace(replace(replace(replace(convert(varchar(100), GetDate(), 121), '-', ''), ' ', ''), ':', ''), '.', '') + '.bak'
--BACKUP DATABASE [RIO_IPS] TO  DISK = @IPS_BK_NAME  WITH NOFORMAT,
--INIT,  NAME = N'RIO_IPS-Full Database Backup', SKIP, NOREWIND, NOUNLOAD, COMPRESSION,  STATS = 10
 
--ENABLE TRIGGER [trg_loggingDDLModify] ON DATABASE
