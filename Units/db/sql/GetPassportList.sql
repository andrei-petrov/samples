CREATE FUNCTION [dbo].[GetPassportList](@ComputerGUID nvarchar(300))
RETURNS  nvarchar(500)
AS
BEGIN

declare @PassportList nvarchar(500);
declare @TabDesc Table(ID int IDENTITY(1,1), Name nvarchar(450) );
declare @Name nvarchar(450);

declare @Count int;
declare @I int;
declare @L int;

set @PassportList = '';

insert into @TabDesc
select P.Name 
from Passport P, PassportLink PL
where P.ObjectGUID = PL.PassportGuid and PL.ParentGUID = @ComputerGUID;
		

select @Count = Count(*) from @TabDesc;

set @I = 1;

while (@I < = @Count)
begin
               select @Name = Name from @TabDesc where ID = @I;
               set @PassportList = @PassportList + @Name + ', ' ;
               set @I = @I + 1; 
end

   -------------  deleting last  ', '  -----------------------------
set @PassportList = RTRIM(@PassportList);
set @L = LEN(@PassportList); 
if (@L >0)
begin 
      set @PassportList = LEFT(@PassportList, @L - 1);
end


	RETURN @PassportList;

END

