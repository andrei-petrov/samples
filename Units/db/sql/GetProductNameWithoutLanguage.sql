CREATE FUNCTION [dbo].[GetProductNameWithoutLanguage](@ProductName nvarchar(450))
RETURNS  nvarchar(1000)
AS
BEGIN

declare @DefiniteLanguage nvarchar(450);
declare @TabProduct Table(Product nvarchar(450) collate Cyrillic_General_CI_AS );
declare @TabLanguage Table(Language nvarchar(450) collate Cyrillic_General_CI_AS );
declare @ProductLanguages Table(ID int IDENTITY(1,1), Language nvarchar(300) collate Cyrillic_General_CI_AS );
declare @ProductCount int;
declare @LanguageCount int;
declare @I int;
declare @L int;
declare @S nvarchar(10);

set @ProductCount = 0;
set @LanguageCount = 0;

insert into @TabProduct values(@ProductName);

insert into @TabLanguage 
select  '%' + LanguageDesc + '%'  from Languages;

--------   check    for   cuting language product list    -------------------------

select @ProductCount = Count(Product) from @TabProduct, ProductList PL
where Product like PL.ProductName;

if (@ProductCount = 0)
begin
        --------   check    for   language in every place of product name    -------------------------
        insert into @ProductLanguages
        select L.LanguageDesc  from @TabProduct P, @TabLanguage TL, Languages L
        where (P.Product like TL.Language) and  (L.LanguageDesc  like TL.Language);

        select @LanguageCount = Count(*) from @ProductLanguages;

        if (@LanguageCount > 0)
        begin
                        set @I = 1;
   
                        while (@I < = @LanguageCount)
                        begin
                                  select @DefiniteLanguage = Language from @ProductLanguages
                                  where ID = @I; 

                                  set @ProductName =  Replace(@ProductName, @DefiniteLanguage, '');
                                  set @ProductName =  Replace(@ProductName, '()', '');
                                  set @ProductName =  Replace(@ProductName, '[]', '');
                                  set @ProductName =  RTRIM(@ProductName);
                                  set @ProductName =  LTRIM(@ProductName);

                                   set @L = LEN(@ProductName);
                                   set @S = SUBSTRING(@ProductName, @L, 1);

                                  while (@S = ',' or @S = '-' or @S = ':'  or @S = ';' or @S = '/')  
                                   begin
                                              set @ProductName = LEFT(@ProductName, @L - 1);

                                              set @L = LEN(@ProductName);
                                              set @S = SUBSTRING(@ProductName, @L, 1);
                                   end
                
                                   set @I = @I + 1; 
                        end
        end
        
                   --------------------   deleting of language code from ProductName end ------------------------------

                       set @L = LEN(@ProductName);
                       
                       set @S = SUBSTRING(@ProductName, @L-2, 3);
                       if  (@S = ' en' or @S = ' fr' or @S = ' es' or @S = ' uk')
                       begin
                               set @ProductName = LEFT(@ProductName, @L - 3); 
                       end
                       else
                       begin
                               set @S = SUBSTRING(@ProductName, @L-3, 4);
                               if  (@S = ' enu' or @S = ' rus' 
                                    or @S = ' /en' or @S = ' /fr' or @S = ' /es' or @S = ' /uk'
                                    or @S = ' -en' or @S = ' -fr' or @S = ' -es' or @S = ' -uk')
                               begin
                                     set @ProductName = LEFT(@ProductName, @L - 4); 
                               end 
                       end 

                       set @L = LEN(@ProductName);
                       set @S = SUBSTRING(@ProductName, @L, 1);
                       if  (@S = ',' or @S = '-' or @S = ':'  or @S = ';')
                       begin
                             set @ProductName = LEFT(@ProductName, @L - 1);
                       end 
                       set @ProductName =  RTRIM(@ProductName);
       
end

RETURN @ProductName;

END