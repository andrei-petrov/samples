IF  EXISTS (SELECT top(1) * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[GetWideSpreadHardware]') AND type in (N'P', N'PC'))
DROP PROCEDURE [dbo].[GetWideSpreadHardware]

EXECUTE(N'
CREATE PROCEDURE [dbo].[GetWideSpreadHardware] 
(@Class nvarchar(450),  @DeviceAmount int)

AS
BEGIN
	
	SET NOCOUNT ON;

declare @Message  nvarchar(1000);
BEGIN TRY  

SELECT  TOP (@DeviceAmount) H.DeviceDesc, COUNT(HI.ObjectGUID) AS InstallationCount,
                ROW_NUMBER() OVER(ORDER BY COUNT(HI.ObjectGUID) DESC) Number
                FROM     Hardware AS H INNER JOIN
                               HardInstallation AS HI ON H.ObjectGUID = HI.HardwareGUID INNER JOIN
                                   (SELECT  ObjectGUID AS HardInstallationGUID
                                    FROM     HardInstallation
                                    WHERE  (ParentGUID IN
                                                       (SELECT  A.ObjectGUID AS InspectionGUID
                                                        FROM     Action AS A INNER JOIN
                                                                           (SELECT  MAX(ActionDate) AS MaxInspectionDate, ParentGUID AS ComputerGUID
                                                                            FROM     Action
                                                                            WHERE  (ActionType = 1) AND (ResultCode = 1)
                                                                            GROUP BY ParentGUID) AS S_1 ON A.ActionDate = S_1.MaxInspectionDate AND A.ParentGUID = S_1.ComputerGUID))) AS S2 ON 
                               HI.ObjectGUID = S2.HardInstallationGUID
                WHERE  (H.Class = @Class) AND (H.IsVisible = 1)
                GROUP BY H.DeviceDesc
                ORDER BY InstallationCount DESC

END TRY
BEGIN CATCH

    Set @Message = Error_Message();
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH
	
END

')
