CREATE PROCEDURE [dbo].[GetComputerOSList] 
(@ComputerGUID nvarchar(50))

AS
BEGIN

	SET NOCOUNT ON;
	
declare @Message     nvarchar(500);

SET DATEFORMAT ymd;

BEGIN TRY

if ( @ComputerGUID is not NULL ) 
begin 
	select 	
       ------------------------------------------------------     
       -- Attention! 
       -- Columns listed below should correspond to the fields 
       -- in InsertNewSoft stored procedure 'OS_List' section.
       -- So if you change this list somehow
       -- you should also change it in  InsertNewSoft.sql
       -- which is needed to import data from xml to db.
       ------------------------------------------------------    	
		    st.ProductName AS N'DisplayName'
	from 
			Soft s, 
			SoftType st,
			Installation i, 
			Action a 
	where 
			(s.ObjectGUID = i.SoftGUID) AND 
			(i.ParentGUID = a.ObjectGUID) AND
			(s.SoftTypeGUID = st.ObjectGUID) AND  
			(a.ParentGUID = @ComputerGUID) AND 
			(s.IsOperatingSystem = 2) AND 
			(a.ResultCode = 1) AND 
			(a.ActionDate = (select MAX(ActionDate) FROM Action WHERE ParentGUID = @ComputerGUID)) 
	order by st.ProductName
end 

END TRY
BEGIN CATCH

    Set @Message = Error_Message();
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
