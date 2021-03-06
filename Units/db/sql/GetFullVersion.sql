CREATE FUNCTION [dbo].[GetFullVersion](@VerMaj nvarchar(50), @VerMin  nvarchar(50), 
                                      @VerBuildMaj nvarchar(150), @VerBuildMin  nvarchar(150))
RETURNS  nvarchar(400)
AS
BEGIN

declare @FullVersion nvarchar(400);
set @FullVersion = '';


    if ( @VerMin   <> '' and @VerMaj   <> '' )
         begin
                set @FullVersion = @VerMaj + '.' + @VerMin;
         end
    else  if ( @VerMin   <> ''  )
         begin
                set @FullVersion = @VerMin;
         end
    else if (  @VerMaj   <> '' )
          begin
                set @FullVersion = @VerMaj;
          end



    if ( @VerBuildMin   <> '' and @VerBuildMaj   <> '' )
         begin
                set @FullVersion = @FullVersion + '.' + @VerBuildMaj + '.' + @VerBuildMin;
         end
    else  if ( @VerBuildMin   <> ''  )
         begin
                set @FullVersion = @FullVersion + '.' + @VerBuildMin;
         end
    else if (  @VerBuildMaj   <> '' )
          begin
                set @FullVersion = @FullVersion + '.' + @VerBuildMaj;
          end


    if(@FullVersion = '' or @FullVersion is NULL)
    begin
         select @FullVersion = DefaultVersion from ObjectRoot;
    end

	RETURN @FullVersion;

END