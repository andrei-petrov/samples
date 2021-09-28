

CREATE VIEW ViewSynonym  
AS
select 'Synonym' as ObjectType,  ParentGUID, ObjectGUID, AttributeName, DisplayName,
	   Replace(Replace(Template, '%%', '*'), '%', '*') as Template  
from  Synonym;


