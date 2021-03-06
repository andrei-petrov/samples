CREATE FUNCTION [dbo].[GetNameFromTemplate](@ObjectName nvarchar(450), @AttributeName nvarchar(100))
RETURNS  nvarchar(1000)
AS
BEGIN

declare @NewObjectName nvarchar(450);
declare @TabObject Table(ObjectName nvarchar(450)  collate Cyrillic_General_CI_AS, 
                         AttributeName nvarchar(100)  collate Cyrillic_General_CI_AS);

set @NewObjectName = '';

insert into @TabObject values(@ObjectName, @AttributeName);

select  @NewObjectName  = Syn.DisplayName
from @TabObject Obj, Synonym Syn
where Obj.ObjectName like Syn.Template
      and Obj.AttributeName = Syn.AttributeName;

if (@NewObjectName  = '' or @NewObjectName is NULL)
begin
          set @NewObjectName = @ObjectName;    
end

RETURN @NewObjectName;

END