CREATE PROCEDURE [dbo].[GetInspectedComputers] 

AS
BEGIN

	SET NOCOUNT ON;
	
declare @Message     nvarchar(500);

SET DATEFORMAT ymd;

BEGIN TRY

select 
	c.ObjectGUID, c.Description, c.Name, MAX(a.ActionDate) AS Date 
from 
	Computer c, 
	Action a 
where a.ParentGUID = c.ObjectGUID
group by c.ObjectGUID, c.Name, c.Description


END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'БД: ошибка запроса проинспектированных компьютеров';
	else
	   set @Message = Error_Message();
   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END

