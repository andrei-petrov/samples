declare @str varchar(255) = '\\localhost\server'

set nocount on
begin tran


-- ��������� �������� ���� slash
 if right(@str, 1) <> '\' 
	begin
		set @str = @str + '\' 
	end

 begin try
	-- ��������� ������ OCR
	update [DELICE_TECH_PROCESS].[dbo].[DT_OCR_SETTINGS] with(rowlock) 
		set 
			HOCR_PATH = @str +  'Hocr', 
			TEXT_PATH  = @str + 'Text'  
		where PROJECT_ID = 1 				-- ��������� ������ OCR
	
	-- ��������� ������ ���������
	update [DELICE_TECH_PROCESS].[dbo].[DT_PICTURES_SORT_SETTINGS] with(rowlock) 
		set 
			SAMPLES_PATH = @str + 'Templates' 
		where PROJECT_ID = 1

	-- ��������� ��������
	update [DELICE_TECH_PROCESS].[dbo].[DT_PROJECTS] with(rowlock) 
		set	PROJECT_PATH = @str + 'ProjectSource',
			PROJECT_SOURCE_PATH = @str + 'CentralServer',
			PROJECT_SCALED_PATH = @str + 'SmallFiles',
			PROJECT_THUMBNAILS_PATH = @str + 'Thumbnails'
		 where PROJECT_ID = 1

	-- ���� ������������ PDF
	update [RIO_IPS].[dbo].[DT_PDF_PATHS] with(rowlock) 
		set PDF_PATH = @str + 'Pdf'
		where PDF_PATH_ID in (select PDF_PATH_ID from RIO_IPS.dbo.DT_PROJECTS (nolock) where PROJECT_ID = 1) 

	-- ��� ������ ��������� ���� � �������� 
	update [RIO_IPS].[dbo].[ST_GBL_CONSTANTS] with(rowlock) 
		set VALUE_CONSTANT = @str +  'Templates' 
		where ABBR = 'SAMPLES_PATH'

	-- ���� � ������������� ��������� ���������
	update [RIO_IPS].[dbo].[ST_GBL_CONSTANTS] with(rowlock) 
		set VALUE_CONSTANT = @str +  'ComplectPage\sc.jpg' 
		where ABBR = 'SEPARATOR_FILE_PATH'

end try
begin catch
	print ERROR_MESSAGE()
	rollback tran
	return
end catch

commit tran