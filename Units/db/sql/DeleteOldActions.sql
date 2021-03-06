CREATE PROCEDURE [dbo].[DeleteOldActions]  
(@XMLObject nvarchar(max))

AS
BEGIN
	
	SET NOCOUNT ON;
	  

SET DATEFORMAT ymd;

declare @Message  nvarchar(500);
declare @XMLDoc int;
  

BEGIN TRY

set @XMLObject = RTRIM(@XMLObject);  
EXEC sp_xml_preparedocument @XMLDoc OUTPUT, @XMLObject;
                
                     
select * into #ObjectInfoTbl
from openxml(@XMLDoc, '/Object/Action', 1)
    with ( ObjectGUID  uniqueidentifier);

      
	delete  A  from Action A
    inner join  #ObjectInfoTbl O on O.ObjectGUID = A.ObjectGUID
	where O.ObjectGUID = A.ObjectGUID;
          

EXEC sp_xml_removedocument @XMLDoc;

delete #ObjectInfoTbl;


END TRY
BEGIN CATCH

    EXEC sp_xml_removedocument @XMLDoc;
    
    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при удалении инспекций';
	else
	   set @Message = Error_Message();
   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH
	
END