
CREATE VIEW ViewLastSuccessComputerInsp
AS
SELECT  MAX(ActionDate) as LastSuccessInspDate, ParentGUID  as ComputerGUID  
FROM   Action 
WHERE  ActionType = 1 AND ResultCode = 1
GROUP BY   ParentGUID






