
CREATE VIEW ViewMaxFilesSearchComputerAction
AS
SELECT  A.IP, A.ObjectGUID AS InspectionGUID, A.Architecture, A.Domain, A.Virtuality, A.ActionDate,
        C.ObjectGUID as ComputerGUID, C.Name, C.Description, C.SubdivisionGUID, Sv.Name as SubdivisionName 
FROM  Action A INNER JOIN 
Computer C ON C.ObjectGUID = A.ParentGUID  LEFT JOIN 
Subdivision Sv ON C.SubdivisionGUID = Sv.ObjectGUID INNER JOIN
	(SELECT  MAX(ActionDate) as MaxDate, ParentGUID    
	FROM   Action 
	WHERE  ActionType = 1 AND ResultCode = 1 and FileSearch = 1
	GROUP BY   ParentGUID)  S
ON A.ParentGUID = S.ParentGUID and A.ActionDate = S.MaxDate


