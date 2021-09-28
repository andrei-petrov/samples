
CREATE VIEW ViewLastComputerInsp
AS
SELECT  MAX(ActionDate) as LastInspectionDate, ParentGUID  as ComputerGUID  
FROM   Action 
WHERE  ActionType = 1 
GROUP BY   ParentGUID


