CREATE FUNCTION [dbo].[GetProductNameGroup](@ProductName nvarchar(300))
RETURNS  nvarchar(1000)
AS
BEGIN

declare @Name nvarchar(450);
declare @LastNamePart nvarchar(450);
declare @I int;
declare @K int;
declare @L int;
declare @L1 int;
declare @S nvarchar(100);
declare @S1 nvarchar(100);
declare @S2 nvarchar(100);
declare @NS nvarchar(100);
declare @LS nvarchar(500);
declare @Len int;
declare @Len2 int;
declare @M int;
declare @Fl bit;
 
set @NS = '0123456789';
set @LS = 'abcdefghijflmnopqrstuvwxyzабвгдежзиклмнопрстуфхцчшщыэюя!%@/#$^&*+|~№?\';
set @Name = '';
set @Fl = 0;

set @L =  CHARINDEX('Vol', @ProductName);
if (@L <> 0)
begin
       set @Fl = 1;
end
set @L = CHARINDEX('Build', @ProductName);
if (@L <> 0)
begin
       set @Fl = 1;
end
set @L = CHARINDEX('CLR', @ProductName);
if (@L <> 0)
begin
       set @Fl = 1;
end
set @L = CHARINDEX('Update', @ProductName);
if (@L <> 0)
begin
       set @Fl = 1;
end

if (@Fl = 0)
begin

                       set @Len = LEN(@ProductName);
                       set @I = 3;  --  from third character 
                       while (@I <= @Len)
                       begin
                                  set @S = SUBSTRING(@ProductName, @I, 1);
                                  set @S1 = SUBSTRING(@ProductName, @I-1, 1);
                                 set @S2 = SUBSTRING(@ProductName, @I-2, 1);         --  for constructions like '... vX'
                                  set @L =  CHARINDEX(@S, @NS)     --       is   character   a number?
                                  if (@L > 0 and (@S1= ' '  or  ((@S1 = 'v' or @S1 = '(' ) and (@S2 = ' ' or @S2 = '(' ))))                 --       is   character   a number and is there a space character before it (to avoid such nember deleting as MP3 etc.)?    
                                  begin     
                                          set @Fl = 0;  
                                            ---------------------------------------------------------------------------------------------------------------------------------                    
                                            ---------------------------------  check for words abcense after number of version --------------------------------------------
                                           set @LastNamePart = Right(@ProductName, @Len-@I);
                                           set @Len2 = LEN(@LastNamePart);
                                           set @K = 1;  
                                           while (@K<= @Len2)
                                           begin 

                                                      set @S = SUBSTRING(@LastNamePart , @K, 1);
                                                      set @L =  CHARINDEX(@S, @LS)     --       is   character   a letter?

                                                      if (@L > 0)  
                                                      begin
                                                               set @Fl = 1;
                                                               break;
                                                      end

                                                      set @K = @K + 1;
                                            end                                                                 
                                            -------------------------------------------------------------------------------------------------------------------------------          
                                            --------------------------   check for number is not a year  -------------------------------------------------------------------

                                            set @LastNamePart = Right(@ProductName, @Len-@I +1);                                            
                                            set @S = SUBSTRING(@LastNamePart , 1, 2);
                                            set @S1 = SUBSTRING(@LastNamePart , 3, 1);
                                            set @S2 = SUBSTRING(@LastNamePart , 4, 1);
                                            set @L =  CHARINDEX(@S1, @NS)     --       is   character   a number?
                                            set @L1 =  CHARINDEX(@S2, @NS)     --       is   character   a number?
                                            if (@S = '20' and @L > 0 and @L > 0)
                                            begin
                                                      set @Fl = 1;    
                                            end

                                           if (@Fl = 0)            --  if   words abcent after number of version      and  number is not a year
                                           begin
                                                        set @Name = Left(@ProductName, @I-1);        --  product name part  before the number
                                                        set @Name = Replace(Replace(@Name, '(', ''), ')', ''); 

                                                        set @L = LEN(@Name);
                                                        if (@L>2)
                                                        begin
                                                                set @S = SUBSTRING(@Name, @L-1, 2);
                                                                if  (@S = '--')
                                                                begin
                                                                      set @Name = LEFT(@Name, @L - 2);
                                                                      set @Name =  RTRIM(@Name);
                                                                end 
                                                         end 

                                                         set @L = LEN(@Name);
                                                          if(@L >1)
                                                          begin
                                                                set @S = SUBSTRING(@Name, @L, 1);
                                                                if  (@S = ',' or @S = '-' or @S = '.' or @S = ':'  or @S = ';'   or @S = '[' or @S = '\' or @S = '/')
                                                                begin
                                                                      set @Name = LEFT(@Name, @L - 1);
                                                                      set @Name =  RTRIM(@Name);
                                                                end  
                                                           end 

                                                         set @L = LEN(@Name);                                     -- deleting 'версия' from the end of the string
                                                         if (@L>7)
                                                         begin
                                                                  set @S = SUBSTRING(@Name, @L-6, 7);
                                                                  if  (@S = ' версия')
                                                                  begin
                                                                           set @Name = LEFT(@Name, @L - 7);
                                                                           set @Name =  RTRIM(@Name);
                                                                  end
                                                          end

                                                          set @L = LEN(@Name);                                      -- deleting 'version' from the end of the string
                                                          if (@L>8)
                                                          begin
                                                                    set @S = SUBSTRING(@Name, @L-7, 8);
                                                                    if  (@S = ' version')
                                                                    begin
                                                                            set @Name = LEFT(@Name, @L - 8);
                                                                            set @Name =  RTRIM(@Name);  
                                                                    end
                                                           end

                                                          set @L = LEN(@Name);                                      -- deleting 'for' from the end of the string
                                                          if (@L>4)
                                                          begin
                                                                    set @S = SUBSTRING(@Name, @L-3, 4);
                                                                    if  (@S = ' for')
                                                                    begin
                                                                            set @Name = LEFT(@Name, @L - 4);
                                                                            set @Name =  RTRIM(@Name);  
                                                                    end
                                                           end  

                                                          set @L = LEN(@Name);                                          -- deleting of the solo 1 last symbol
                                                          if (@L > 2)
                                                          begin
                                                                    set @S = SUBSTRING(@Name, @L-1, 1);
                                                                     if  (@S = ' ')
                                                                     begin
                                                                            set @Name = LEFT(@Name, @L - 1);
                                                                            set @Name =  RTRIM(@Name); 
                                                                     end  
                                                           end

                                                          set @L = LEN(@Name);
                                                          if(@L >1)
                                                          begin
                                                                set @S = SUBSTRING(@Name, @L, 1);
                                                                if  (@S = ',' or @S = '-' or @S = '.' or @S = ':'  or @S = ';'   or @S = '[' or @S = '\' or @S = '/')
                                                                begin
                                                                      set @Name = LEFT(@Name, @L - 1);
                                                                      set @Name =  RTRIM(@Name);
                                                                end  
                                                           end 
                                       
                                             break;

                                            end           
                                end 
           
                                 set @I = @I + 1;   
                     end   
end            


           if (@Name  = '' or @Name is  NULL)      
           begin
                      set @Name = @ProductName;
           end

RETURN  @Name;

END

