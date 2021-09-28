create or alter procedure ADD_USER (@UserName varchar(128), @LoginNT varchar(128), @result nvarchar(4000) out)
as
/* Добавление пользователя в ряды администраторов системы

declare @i int
declare @UserName varchar(128) = 'Тест  ' 
declare @LoginNT varchar(128) = 'HOST\NT_TEST2'
declare @result nvarchar(4000) 

exec @i = ADD_USERNAME @UserName, @LoginNT, @result out
select @i as I, @result as RES 

*/
begin

set nocount on
set transaction isolation level read uncommitted

declare @NAME_GROUP VARCHAR(128) = ''
declare @ID_GROUP INT = 0
declare @ID_LOGIN INT = 0



select @NAME_GROUP = t2.GROUP_SHORT_NAME, @ID_GROUP = t2.ID_GROUP  from ST_LOGINS t 
join SS_GROUP_MEMBERS t1 on t1.ID_LOGIN = t.ID_LOGIN
join SS_GROUPS t2 on t2.ID_GROUP = t1.ID_GROUP
where t.LOGIN_VALUE  = 'sa'
if @@ROWCOUNT = 0
begin
	set @result = 'Административная группа не определеня'
	return 0
end

begin try
	INSERT INTO ST_LOGINS (STATE_LOGIN,LOGIN_VALUE, NAME_USER) VALUES ('A',@LoginNT, @UserName)
	SET @ID_LOGIN = SCOPE_IDENTITY()
end try
	begin catch
		set @result = ERROR_MESSAGE()
		return 0
	end catch

begin tran

begin try
	INSERT INTO [dbo].[ST_GRANT_APPLICATION_USER] (ID_LOGIN, ID_APPLICATION, ACCESS_MASK)
	SELECT @ID_LOGIN, ID_APPLICATION, 1
	FROM [dbo].[SS_APPLICATION]
end try
begin catch
		set @result = ERROR_MESSAGE()
		rollback tran		
		return 0
end catch

begin try

INSERT INTO [SS_GROUP_MEMBERS] (ID_GROUP, ID_LOGIN) VALUES (@ID_GROUP, @ID_LOGIN)

delete from st_grant_application_user where ID_LOGIN = @ID_LOGIN
delete from st_grant_procedure_user where ID_LOGIN = @ID_LOGIN
delete from st_grant_report_user where ID_LOGIN = @ID_LOGIN
delete from st_grant_report_ole_user where ID_LOGIN = @ID_LOGIN
delete from st_grant_report_office_user where ID_LOGIN = @ID_LOGIN
delete from st_grant_object_filter_user where ID_LOGIN = @ID_LOGIN
delete from st_grant_object_fields_user where ID_LOGIN = @ID_LOGIN
delete from st_grant_object_user where ID_LOGIN = @ID_LOGIN

insert into st_grant_application_user (ID_LOGIN, ID_APPLICATION, ACCESS_MASK, RUN_OPTIONS)
select @ID_LOGIN, ID_APPLICATION, ACCESS_MASK, RUN_OPTIONS from st_grant_application_groupe where (ID_GROUP = 1)and
(not (ID_APPLICATION in (select ID_APPLICATION from st_grant_application_user where ID_LOGIN = @ID_LOGIN)))

insert into st_grant_procedure_user (ID_LOGIN, ID_PROCEDURE, ACCESS_MASK)
select @ID_LOGIN, ID_PROCEDURE, ACCESS_MASK from st_grant_procedure_groupe where (ID_GROUP = 1)and
(not (ID_PROCEDURE in (select ID_PROCEDURE from st_grant_procedure_user where ID_LOGIN = @ID_LOGIN)))

insert into st_grant_report_user (ID_LOGIN, ID_REPORTS, ACCESS_MASK)
select @ID_LOGIN, ID_REPORTS, ACCESS_MASK from st_grant_report_groupe where (ID_GROUP = 1)and
(not (ID_REPORTS in (select ID_REPORTS from st_grant_report_user where ID_LOGIN = @ID_LOGIN)))

insert into st_grant_report_ole_user (ID_LOGIN, ID_REPORTS_OLE, ACCESS_MASK)
select @ID_LOGIN, ID_REPORTS_OLE, ACCESS_MASK from st_grant_report_ole_groupe where (ID_GROUP = 1)and
(not (ID_REPORTS_OLE in (select ID_REPORTS_OLE from st_grant_report_ole_user where ID_LOGIN = @ID_LOGIN)))

insert into st_grant_report_office_user (ID_LOGIN, ID_REPORT, ACCESS_MASK)
select @ID_LOGIN, ID_REPORT, ACCESS_MASK from st_grant_report_office_groupe where (ID_GROUP = 1)and
(not (ID_REPORT in (select ID_REPORT from st_grant_report_office_user where ID_LOGIN = @ID_LOGIN)))

insert into st_grant_object_user (ID_LOGIN, SYS_NAME, ACCESS_MASK)
select @ID_LOGIN, SYS_NAME, ACCESS_MASK from st_grant_object_groupe where (ID_GROUP = 1)and
(not (SYS_NAME in (select SYS_NAME from st_grant_object_user where ID_LOGIN = @ID_LOGIN)))

insert into st_grant_object_filter_user (ID_LOGIN, ID_FILTER, ACCESS_MASK)
select @ID_LOGIN, ID_FILTER, ACCESS_MASK from st_grant_object_filter_groupe where (ID_GROUP = 1)and
(not (ID_FILTER in (select ID_FILTER from st_grant_object_filter_user where ID_LOGIN = @ID_LOGIN)))

insert into st_grant_object_fields_user
(ID_LOGIN, SYS_NAME, CODE_FIELD, ACCESS_MASK, VISIBLE_VIEW, VISIBLE_EDIT, FOR_READ, TYPE_FILL, VALUE_DEFAULT,TABLE_NAME, LOOK_FIELD, KEY_FIELD, INDEX_FIELDS, FILTERS, SQL_STRING)
select @ID_LOGIN, SYS_NAME, CODE_FIELD, ACCESS_MASK, VISIBLE_VIEW, VISIBLE_EDIT, FOR_READ, TYPE_FILL, VALUE_DEFAULT,TABLE_NAME, LOOK_FIELD, KEY_FIELD, INDEX_FIELDS, FILTERS, SQL_STRING from st_grant_object_fields_groupe T0
where (ID_GROUP = 1)and( not exists( select 1 from st_grant_object_fields_user T
where (T.ID_LOGIN = @ID_LOGIN)and(T.SYS_NAME = T0.SYS_NAME)and(T.CODE_FIELD = T0.CODE_FIELD)))

end try

begin catch
		set @result = ERROR_MESSAGE()
		rollback tran
		return 0
end catch

commit tran
set @result = 'OK'
return 1

end