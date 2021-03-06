---------------------------------------------------------------------------------------------------------
-- Author:      Dmitry Karpov
-- Date:        01.10.2009
-- Notes:       Return soft for computer from the last successfull inspection.
--              Need to export xml reports from db.
-- Parameters:  
--              @ComputerGUID - guid of computer
---------------------------------------------------------------------------------------------------------
CREATE PROCEDURE [dbo].[GetComputerSoft] 
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
       -- in InsertNewSoft stored procedure 'Installed Soft' section.
       -- So if you change this list somehow
       -- you should also change it in  InsertNewSoft.sql
       -- which is needed to import data from xml to db.
       ------------------------------------------------------    	
		    st.ProductName, 
		    st.VersionMajor,
		    s.VersionMinor, 
		    p.Name as Publisher, 
		    s.Localization AS N'Language',
		    s.ProductKey  	
	from 
			Soft s,
			SoftType st, 
			Installation i, 
			Action a,
			Publisher p 
	where 
			(s.ObjectGUID = i.SoftGUID) AND 
			(st.PublisherGUID = p.ObjectGUID) AND
			(i.ParentGUID = a.ObjectGUID) AND
			(s.SoftTypeGUID = s.ObjectGUID) AND  
			(a.ParentGUID = @ComputerGUID) AND 
			(s.IsOperatingSystem = 0) AND 
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