---------------------------------------------------------------------------------------------------------
-- Author:      Dmitry Karpov
-- Date:        July 8, 2009
-- Notes:       This procedure checks if objectGuid is operable.
--              Object might not be operable because it can be a descendant of running task.
-- Parameters:  
--              @ObjectGuid - guid of task to check for operability
--              @TaskGuidList - comma separeted running task's guid list
--              @IsLocked - output bit value  
---------------------------------------------------------------------------------------------------------

CREATE PROCEDURE [dbo].[IsObjectLocked] 
(@ObjectGuid nvarchar(50),  @TaskGuidList nvarchar(max),  @IsLocked bit  output)

AS
BEGIN
	
	SET NOCOUNT ON;

	declare @Message  nvarchar(1000);
	declare @Object_Count int;
BEGIN TRY  


	DECLARE @RunningTasks TABLE (ObjectGuid uniqueidentifier);

	-- fill out @RunningTasks table from comma separated guid list
	DECLARE @TaskGuid varchar(50), @Pos int

	SET @TaskGuidList = LTRIM(RTRIM(@TaskGuidList))+ ','
	SET @Pos = CHARINDEX(',', @TaskGuidList, 1)

	IF REPLACE(@TaskGuidList, ',', '') <> ''
	BEGIN
		WHILE @Pos > 0
		BEGIN
			SET @TaskGuid = LTRIM(RTRIM(LEFT(@TaskGuidList, @Pos - 1)))
			IF @TaskGuid <> ''
			BEGIN
				INSERT INTO @RunningTasks (ObjectGuid) VALUES (CAST(@TaskGuid AS uniqueidentifier)) --Use Appropriate conversion
			END
			SET @TaskGuidList = RIGHT(@TaskGuidList, LEN(@TaskGuidList) - @Pos)
			SET @Pos = CHARINDEX(',', @TaskGuidList, 1)

		END
	END	

	-- store Tasks and their Computers
	DECLARE @TasksAndDescendants TABLE (ObjectGuid uniqueidentifier);

	insert INTO @TasksAndDescendants 
	-- getComputerLinks for task
	select CL.ObjectGuid from ComputerLink CL, @RunningTasks RunningTasks 
	where CL.ParentGUID = RunningTasks.ObjectGuid 
	UNION
	-- get computers for task using computerLinks
	select ComputerGuid as ObjectGuid from ComputerLink, @RunningTasks RunningTasks
	where ComputerLink.ParentGUID = RunningTasks.ObjectGuid 
	UNION 
	-- get task itself
	select ObjectRelations.ObjectGuid from ObjectRelations, @RunningTasks RunningTasks
	where ObjectRelations.ObjectGuid = RunningTasks.ObjectGuid;
	

	Select @Object_Count = count(*) from @TasksAndDescendants
	Where objectGuid = @ObjectGuid;

	if (@Object_Count > 0)
	begin
		set @IsLocked = 1;
	end
	else
	begin
		set @IsLocked = 0;
	end
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при запросе о блокировке объекта';
	else
	   set @Message = Error_Message();
	   
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH
	
END
