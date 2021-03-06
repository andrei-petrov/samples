IF  EXISTS (SELECT top(1) * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[GetCompWithSecurity]') AND type in (N'P', N'PC'))
DROP PROCEDURE [dbo].[GetCompWithSecurity]

EXECUTE(N'
CREATE PROCEDURE [dbo].[GetCompWithSecurity] 
(@SubdivisionName nvarchar(1000))

AS
BEGIN
	
	SET NOCOUNT ON;

declare @Message  nvarchar(1000);
BEGIN TRY  

declare @N int;

select @N = count(Name) from ViewMaxComputerAction V
WHERE (V.SubdivisionName = @SubdivisionName)
		OR  ((V.SubdivisionGUID IS NULL) AND (@SubdivisionName = ''Компьютеры вне подразделений'')) 
		OR  (@SubdivisionName = ''Все подразделения'');

if (@N > 0)
begin


		select count(S2.ComputerName)* 100/@N  as ComputerPercent,  (100 - count(S2.ComputerName)* 100/@N) as NotHaving,
        S2.CategoryName as Category, ROW_NUMBER() OVER(ORDER BY COUNT(S2.ComputerName) DESC) Number 

		from   (    select  distinct V.Name as ComputerName, C.Name as CategoryName 

					FROM     Soft AS S INNER JOIN
					SoftType AS STP ON STP.ObjectGUID = S.SoftTypeGUID INNER JOIN
					Category AS C ON C.ObjectGUID = STP.CategoryGUID INNER JOIN
					Installation AS I ON S.ObjectGUID = I.SoftGUID INNER JOIN
					ViewMaxComputerAction AS V ON V.InspectionGUID = I.ParentGUID INNER JOIN
                    SecurityTypeLU AS SLu ON C.Name = SLu.SecurityType

					WHERE (V.SubdivisionName = @SubdivisionName)
						OR  ((V.SubdivisionGUID IS NULL) AND (@SubdivisionName = ''Компьютеры вне подразделений'')) 
						OR  (@SubdivisionName = ''Все подразделения'')
				 ) S2
	
        group by S2.CategoryName


end



END TRY
BEGIN CATCH

    Set @Message = Error_Message();
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH
	
END

')