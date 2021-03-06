

CREATE VIEW ViewLibProductFile
AS
select ObjectGUID, 'LibProductFile' as ObjectType,  ParentGUID, InitLibFlag,  OriginalFileName, Size,
 Replace(Replace(FileVersion,  '%%', '*'), '%', '*') as FileVersion,
 Replace(Replace(Company, '%%', '*'), '%', '*') as Company,
 Replace(Replace(ProductName, '%%', '*'), '%', '*') as ProductName, 
 Replace(Replace(ProductVersion, '%%', '*'), '%', '*') as ProductVersion,  
 Replace(Replace(Language, '%%', '*'), '%', '*') as Language
from LibProductFile;

