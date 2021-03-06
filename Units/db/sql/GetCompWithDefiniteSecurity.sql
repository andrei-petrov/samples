IF  EXISTS (SELECT top(1) * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[GetCompWithDefiniteSecurity]') AND type in (N'P', N'PC'))
DROP PROCEDURE [dbo].[GetCompWithDefiniteSecurity]

EXECUTE(N'
CREATE PROCEDURE [dbo].[GetCompWithDefiniteSecurity] 
(@SubdivisionName nvarchar(1000), @CategoryName nvarchar(500))

AS
BEGIN
	
	SET NOCOUNT ON;

declare @Message  nvarchar(1000);

BEGIN TRY 


CREATE TABLE #Computer(  [ComputerGUID] [nvarchar](50)   NULL,
                         [ComputerName] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
                         [InspectionGUID] [nvarchar](50)   NULL,
                         [Virtuality] [nvarchar](300) collate Cyrillic_General_CI_AS  NULL,
                         [Description] [nvarchar](1000) collate Cyrillic_General_CI_AS  NULL,
                         [SoftList] [nvarchar](4000) collate Cyrillic_General_CI_AS  NULL
                     ) ON [PRIMARY];

 
insert into #Computer
SELECT DISTINCT  V.ComputerGUID,  V.Name, V.InspectionGUID, V.Virtuality, V.Description, NULL  
FROM     Soft AS S INNER JOIN
SoftType AS STP ON STP.ObjectGUID = S.SoftTypeGUID INNER JOIN
Category AS C ON C.ObjectGUID = STP.CategoryGUID INNER JOIN
Installation AS I ON S.ObjectGUID = I.SoftGUID INNER JOIN
ViewMaxComputerAction AS V ON V.InspectionGUID = I.ParentGUID
WHERE  C.Name  =  @CategoryName
AND ((V.SubdivisionName = @SubdivisionName) 
	OR  ((V.SubdivisionGUID IS NULL) AND (@SubdivisionName = ''Компьютеры вне подразделений'')) 
	OR  (@SubdivisionName = ''Все подразделения''));



update #Computer
set SoftList = dbo.GetSecuritySoftList(InspectionGUID, @CategoryName);



select ComputerGUID, ComputerName, Virtuality, Description, SoftList,
 ''Компьютеры с ПО заданной категории'' as SecurityDesc, 0 as  SecurityDescOrder
from #Computer 
UNION
select ComputerGUID,  Name as ComputerName, Virtuality, Description, NULL,
''Компьютеры без ПО заданной категории'' as SecurityDesc, 1  as  SecurityDescOrder
from ViewMaxComputerAction 
where Name not in (select  ComputerName from #Computer)
AND ((SubdivisionName = @SubdivisionName)
	OR  ((SubdivisionGUID IS NULL) AND (@SubdivisionName = ''Компьютеры вне подразделений'')) 
	OR  (@SubdivisionName = ''Все подразделения''))
UNION
select ObjectGUID as ComputerGUID, Name as ComputerName, NULL as Virtuality, Description, NULL,
''Непроинспектированные компьютеры'' as SecurityDesc, 2  as  SecurityDescOrder 
from Computer
where Name not in (select Name from ViewMaxComputerAction)
AND ((SubdivisionGUID IN
			(SELECT  ObjectGUID
			 FROM     Subdivision
			 WHERE  Name = @SubdivisionName)) 
	OR  ((SubdivisionGUID IS NULL) AND (@SubdivisionName = ''Компьютеры вне подразделений'')) 
	OR  (@SubdivisionName = ''Все подразделения''))
order by SecurityDescOrder, ComputerName;




END TRY
BEGIN CATCH

    Set @Message = Error_Message();
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH
	
END

')