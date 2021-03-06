---------------------------------------------------------------------------------------------------------
-- Author:      Dmitry Karpov
-- Date:        01.10.2009
-- Notes:       Return hard for computer from the last successfull inspection.
--              Need to export xml reports from db.
-- Parameters:  
--              @ComputerGUID - guid of computer
---------------------------------------------------------------------------------------------------------
CREATE PROCEDURE [dbo].[GetComputerHard] 
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
       -- in InsertNewSoft stored procedure 'Installed Hardware' section.
       -- So if you change this list somehow
       -- you should also change it in  InsertNewSoft.sql
       -- which is needed to import data from xml to db.
       ------------------------------------------------------    		
		h.Class, 
		h.DeviceDesc, 
		'' as FriendlyName,   --  for compatibility whith XML for SP InsertNewSoft
		i.Mfg 	
	from 
		Hardware h, 
		HardInstallation i, 
		Action a 
	where 
		(h.ObjectGUID = i.HardwareGUID) AND 
		(i.ParentGUID = a.ObjectGUID) AND 
		(a.ParentGUID = @ComputerGUID) AND 
		(a.ResultCode = 1) AND 
		(a.ActionDate = (select MAX(ActionDate) FROM Action WHERE ParentGUID = @ComputerGUID))
	order by h.DeviceDesc
end 

END TRY
BEGIN CATCH

    Set @Message = Error_Message();
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
