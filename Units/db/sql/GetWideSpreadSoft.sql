IF  EXISTS (SELECT top(1) * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[GetWideSpreadSoft]') AND type in (N'P', N'PC'))
DROP PROCEDURE [dbo].[GetWideSpreadSoft]

EXECUTE(N'
CREATE PROCEDURE [dbo].[GetWideSpreadSoft] 
(@SoftType int, @ProductAmount int)

AS
BEGIN
	
	SET NOCOUNT ON;

declare @Message  nvarchar(1000);
BEGIN TRY  

SELECT  TOP (@ProductAmount)  STP.ProductName, COUNT(S.ObjectGUID) AS InstallationCount,
                         ROW_NUMBER() OVER(ORDER BY COUNT(S.ObjectGUID) DESC) Number
FROM     Soft AS S INNER JOIN
            SoftType AS STP ON S.SoftTypeGUID = STP.ObjectGUID INNER JOIN   
               Installation AS I ON S.ObjectGUID = I.SoftGUID INNER JOIN
                   (SELECT  ObjectGUID AS InstallationGUID
                    FROM     Installation
                    WHERE  (ParentGUID IN
                                       (SELECT  A.ObjectGUID
                                        FROM     Action AS A INNER JOIN
                                                           (SELECT  MAX(ActionDate) AS MaxInspectionDate, ParentGUID AS ComputerGUID
                                                            FROM     Action
                                                            WHERE  (ActionType = 1) AND (ResultCode = 1)
                                                            GROUP BY ParentGUID) AS S1 ON A.ActionDate = S1.MaxInspectionDate AND A.ParentGUID = S1.ComputerGUID))) AS S2 ON 
               I.ObjectGUID = S2.InstallationGUID  
WHERE  (S.IsVisible = 1) and (S.IsOperatingSystem = @SoftType)
GROUP BY STP.ProductName
ORDER BY InstallationCount DESC


END TRY
BEGIN CATCH

    Set @Message = Error_Message();
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH
	
END

')
