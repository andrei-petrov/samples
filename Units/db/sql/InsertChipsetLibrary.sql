CREATE PROCEDURE [dbo].[InsertChipsetLibrary] 

AS
BEGIN
	
	SET NOCOUNT ON;


SET DATEFORMAT ymd;

declare @XMLObject nvarchar(max)
declare @Message  nvarchar(1000);


BEGIN TRY

     set @XMLObject = '<ChipseInfo>
	 <Vendor Name ="Intel">    
    <Chipset Name ="400 Series" Northbridge="" CodeName=""/> 
    <Chipset Name ="900 Series" Northbridge="" CodeName=""/>
    <Chipset Name ="800 Series" Northbridge="" CodeName=""/>
    <Chipset Name ="7 Series" Northbridge="" CodeName=""/>
    <Chipset Name ="6 Series" Northbridge="" CodeName=""/>
    <Chipset Name ="5 Series" Northbridge="" CodeName=""/>
    <Chipset Name ="4 Series" Northbridge="" CodeName=""/>
    <Chipset Name ="X38" Northbridge="82X38" CodeName="Bearlake"/>
    <Chipset Name ="X48" Northbridge="82X48" CodeName="Bearlake"/>
    <Chipset Name ="G33" Northbridge="82G33" CodeName="Bearlake"/>
    <Chipset Name ="G31" Northbridge="82G31" CodeName="Bearlake"/>
    <Chipset Name ="P35" Northbridge="82P35" CodeName="Bearlake"/>
    <Chipset Name ="P31" Northbridge="82P31" CodeName="Bearlake"/>
    <Chipset Name ="3 Series" Northbridge="" CodeName="Bearlake"/>
    <Chipset Name ="3000" Northbridge="" CodeName="Mukilteo"/>
    <Chipset Name ="3010" Northbridge="" CodeName="Mukilteo 2"/>
    <Chipset Name ="3100" Northbridge="" CodeName="Whitmore Lake"/>
    <Chipset Name ="3200" Northbridge="" CodeName="Bigby"/>
    <Chipset Name ="3210" Northbridge="" CodeName="Bigby"/>
    <Chipset Name ="3400 Series" Northbridge="" CodeName=""/>
    <Chipset Name ="5000 Series" Northbridge="" CodeName="" />
    <Chipset Name ="5100" Northbridge="" CodeName="San Clemente" />
    <Chipset Name ="5000V" Northbridge="" CodeName="Blackford"/>
    <Chipset Name ="5000Z" Northbridge="" CodeName="Blackford"/>
    <Chipset Name ="5000P" Northbridge="" CodeName="Blackford"/>   
    <Chipset Name ="5000X" Northbridge="" CodeName="Greencreek"/>
    <Chipset Name ="5400" Northbridge="" CodeName="Seaburg"/>
    <Chipset Name ="5500" Northbridge="" CodeName="Tylersburg"/>
    <Chipset Name ="5520" Northbridge="" CodeName="Tylersburg"/>
    <Chipset Name ="7300" Northbridge="" CodeName="Clarksboro"/>
    <Chipset Name ="E7205" Northbridge="" CodeName="Granite Bay"/>
    <Chipset Name ="E7210" Northbridge="" CodeName="Canterwood-ES"/>
    <Chipset Name ="E7221" Northbridge="" CodeName="Copper River"/>
    <Chipset Name ="E7230" Northbridge="" CodeName="Mukilteo"/>
    <Chipset Name ="E7320" Northbridge="" CodeName="Lindenhurst"/>
    <Chipset Name ="E7500" Northbridge="" CodeName="Plumas"/>
    <Chipset Name ="E7501" Northbridge="" CodeName="Plumas"/>
    <Chipset Name ="E7505" Northbridge="" CodeName="Placer"/>
    <Chipset Name ="E7510" Northbridge="" CodeName="Lindenhurst VS"/>
    <Chipset Name ="E7515" Northbridge="" CodeName="Tumwater"/>
    <Chipset Name ="E7520" Northbridge="" CodeName="Lindenhurst"/>
    <Chipset Name ="E7525" Northbridge="" CodeName=""/>
    <Chipset Name ="E7710" Northbridge="" CodeName="Lindenhurst"/>
    <Chipset Name ="E8500" Northbridge="" CodeName="Truland"/>
    <Chipset Name ="E8501" Northbridge="" CodeName=""/>
    <Chipset Name ="NM10" Northbridge="" CodeName="Tiger Point"/>
    <Chipset Name ="UL11L" Northbridge="" CodeName="Poulsbo"/>
    <Chipset Name ="US15L" Northbridge="" CodeName="Poulsbo"/>
    <Chipset Name ="US15W" Northbridge="" CodeName="Poulsbo"/>
    <Chipset Name ="US15WP" Northbridge="" CodeName="Poulsbo"/>
    <Chipset Name ="US15WPT" Northbridge="" CodeName="Poulsbo"/>
    <Chipset Name ="440" Northbridge="82443" CodeName="Seattle"/>
    <Chipset Name ="440BX" Northbridge="82443BX" CodeName="Seattle"/>
    <Chipset Name ="450" Northbridge="82451" CodeName=""/>
    <Chipset Name ="450KX" Northbridge="82451KX" CodeName="Mars"/>
    <Chipset Name ="450KX" Northbridge="82452KX" CodeName="Mars"/>
    <Chipset Name ="450KX" Northbridge="82453KX" CodeName="Mars"/>
    <Chipset Name ="450KX" Northbridge="82454KX" CodeName="Mars"/>
    <Chipset Name ="450GX" Northbridge="82451GX" CodeName="Orion"/>
    <Chipset Name ="450GX" Northbridge="82452GX" CodeName="Orion"/>
    <Chipset Name ="450GX" Northbridge="82453GX" CodeName="Orion"/>
    <Chipset Name ="450GX" Northbridge="82454GX" CodeName="Orion"/>
    <Chipset Name ="440FX" Northbridge="82441FX" CodeName="Natoma"/>
    <Chipset Name ="440FX" Northbridge="82442FX" CodeName="Natoma"/>
    <Chipset Name ="440LX" Northbridge="82443LX" CodeName="Balboa"/>
    <Chipset Name ="440EX" Northbridge="82443EX" CodeName=""/>
    <Chipset Name ="440GX" Northbridge="82443GX" CodeName=""/>
    <Chipset Name ="450NX" Northbridge="82451NX" CodeName=""/>
    <Chipset Name ="450NX" Northbridge="82452NX" CodeName=""/>
    <Chipset Name ="450NX" Northbridge="82453NX" CodeName=""/>
    <Chipset Name ="450NX" Northbridge="82454NX" CodeName=""/>
    <Chipset Name ="440ZX" Northbridge="82443ZX" CodeName=""/>
    <Chipset Name ="440ZX-66" Northbridge="82443ZX" CodeName=""/>
    <Chipset Name ="440ZX-M" Northbridge="82443ZX-M" CodeName=""/>
    <Chipset Name ="440MX" Northbridge="82443MX" CodeName="Banister"/>
    <Chipset Name ="810" Northbridge="82810" CodeName="Whitney"/>
    <Chipset Name ="810E" Northbridge="82810E" CodeName="Whitney"/>
    <Chipset Name ="810E" Northbridge="8210E" CodeName="Whitney"/>
    <Chipset Name ="820" Northbridge="82820" CodeName="Camino"/>
    <Chipset Name ="840" Northbridge="82840" CodeName="Carmel"/>
    <Chipset Name ="820E" Northbridge="82820" CodeName="Camino"/>
    <Chipset Name ="815" Northbridge="82815" CodeName="Solano"/>
    <Chipset Name ="815E" Northbridge="82815" CodeName="Solano"/>
    <Chipset Name ="815EP" Northbridge="82815EP" CodeName="Solano"/>
    <Chipset Name ="815P" Northbridge="82815EP" CodeName="Solano"/>
    <Chipset Name ="815G" Northbridge="82815G" CodeName="Solano"/>
    <Chipset Name ="815EG" Northbridge="82815G" CodeName="Solano"/>
    <Chipset Name ="830M" Northbridge="82830M" CodeName="Almador"/>
    <Chipset Name ="830" Northbridge="82830" CodeName="Almador"/>
    <Chipset Name ="830MG" Northbridge="82830MG" CodeName="Almador"/>
    <Chipset Name ="830MP" Northbridge="82830MP" CodeName="Almador"/>
    <Chipset Name ="850" Northbridge="82850" CodeName="Tehama"/>
    <Chipset Name ="860" Northbridge="82860" CodeName="Colusa"/>
    <Chipset Name ="845" Northbridge="82845" CodeName="Brookdale"/>
    <Chipset Name ="850E" Northbridge="82850E" CodeName="Tehama"/>
    <Chipset Name ="845E" Northbridge="82845E" CodeName="Brookdale"/>    
    <Chipset Name ="845GL" Northbridge="82845GL " CodeName="Brookdale"/>
    <Chipset Name ="845G" Northbridge="82845G " CodeName="Brookdale"/>
    <Chipset Name ="845GE" Northbridge="82845GE" CodeName="Brookdale"/>
    <Chipset Name ="845PE" Northbridge="82845PE" CodeName="Brookdale"/>
    <Chipset Name ="845GV" Northbridge="82845GV" CodeName="Brookdale"/>
    <Chipset Name ="875" Northbridge="82875" CodeName="Canterwood"/>
    <Chipset Name ="875P" Northbridge="82875P" CodeName="Canterwood"/>
    <Chipset Name ="865G" Northbridge="82865G" CodeName="Springdale"/>
    <Chipset Name ="865" Northbridge="82865" CodeName="Springdale"/>
    <Chipset Name ="865P" Northbridge="82865P" CodeName="Springdale"/>
    <Chipset Name ="865PE" Northbridge="82865PE" CodeName="Springdale"/>
    <Chipset Name ="848" Northbridge="82848" CodeName="Breeds Hill"/>
    <Chipset Name ="865GV" Northbridge="82865GV" CodeName="Springdale"/>
    <Chipset Name ="845MP" Northbridge="82845" CodeName="Brookdale"/>
    <Chipset Name ="845MZ" Northbridge="82845" CodeName="Brookdale"/>
    <Chipset Name ="852" Northbridge="82852" CodeName="Montara"/>
    <Chipset Name ="852GM" Northbridge="82852GM" CodeName="Montara"/>
    <Chipset Name ="852GMV" Northbridge="82852GMV" CodeName="Montara"/>
    <Chipset Name ="852PM" Northbridge="82852PM" CodeName="Montara"/>
    <Chipset Name ="852GME" Northbridge="82852GME" CodeName="Montara"/>
    <Chipset Name ="854" Northbridge="82854" CodeName=""/>
    <Chipset Name ="855GM" Northbridge="82855GM" CodeName="Montara"/>
    <Chipset Name ="855" Northbridge="82855" CodeName=""/>
    <Chipset Name ="855GME" Northbridge="82855GME" CodeName="Montara"/>
    <Chipset Name ="855PM" Northbridge="82855PM" CodeName="Odem"/>
    <Chipset Name ="910" Northbridge="82910" CodeName="Grantsdale"/>
    <Chipset Name ="910GL" Northbridge="82910GL" CodeName="Grantsdale"/>
    <Chipset Name ="915P" Northbridge="82915P" CodeName="Grantsdale"/>
    <Chipset Name ="915PL" Northbridge="82915PL" CodeName="Grantsdale"/>
    <Chipset Name ="915G" Northbridge="82915G" CodeName="Grantsdale"/>
    <Chipset Name ="915GL" Northbridge="82915GL" CodeName="Grantsdale"/>
    <Chipset Name ="915" Northbridge="82915" CodeName="Grantsdale"/>
    <Chipset Name ="915GV" Northbridge="82915GV" CodeName="Grantsdale"/>
    <Chipset Name ="925" Northbridge="82925" CodeName="Alderwood"/>
    <Chipset Name ="925X" Northbridge="82925X" CodeName="Alderwood"/>
    <Chipset Name ="925XE" Northbridge="82925XE" CodeName="Alderwood"/>
    <Chipset Name ="955X" Northbridge="82945X" CodeName="Lakeport"/>
    <Chipset Name ="955P" Northbridge="82945P" CodeName="Lakeport"/>
    <Chipset Name ="955" Northbridge="82945" CodeName="Lakeport"/>
    <Chipset Name ="955PL" Northbridge="82945PL" CodeName="Lakeport"/>
    <Chipset Name ="955G" Northbridge="82945G" CodeName="Lakeport"/>
    <Chipset Name ="910GML" Northbridge="82910GML" CodeName="Alviso"/>
    <Chipset Name ="915GMS" Northbridge="82915GMS" CodeName="Alviso"/>
    <Chipset Name ="915GM" Northbridge="82915GM" CodeName="Alviso"/>
    <Chipset Name ="915PM" Northbridge="82915PM" CodeName="Alviso"/>
    <Chipset Name ="940GML" Northbridge="82940GML" CodeName="Calistoga"/>
    <Chipset Name ="940" Northbridge="82940" CodeName="Calistoga"/>
    <Chipset Name ="943GML" Northbridge="82943GML" CodeName="Calistoga"/>
    <Chipset Name ="943" Northbridge="82943" CodeName="Calistoga"/>
    <Chipset Name ="945" Northbridge="82945" CodeName=""/>
    <Chipset Name ="945G" Northbridge="82945G" CodeName="Lakeport"/>
    <Chipset Name ="945GMS" Northbridge="82945GMS" CodeName="Calistoga"/>
    <Chipset Name ="945GSE" Northbridge="82945GSE" CodeName="Calistoga"/>
    <Chipset Name ="945GM/E" Northbridge="82945GM/E" CodeName="Calistoga"/>
    <Chipset Name ="945PM" Northbridge="82945PM" CodeName="Calistoga"/>
    <Chipset Name ="945GC" Northbridge="82945GC" CodeName="Lakeport"/>
    <Chipset Name ="945GZ" Northbridge="82945GZ" CodeName="Lakeport"/>
    <Chipset Name ="946PL" Northbridge="82946PL" CodeName="Lakeport"/>
    <Chipset Name ="946GZ" Northbridge="82946GZ " CodeName="Lakeport"/>
    <Chipset Name ="946" Northbridge="82946" CodeName="Lakeport"/>
    <Chipset Name ="975X" Northbridge="82975X" CodeName="Glenwood"/>
    <Chipset Name ="975" Northbridge="82975" CodeName="Glenwood"/>
    <Chipset Name ="P965" Northbridge="82P965" CodeName="Broadwater"/>
    <Chipset Name ="G965" Northbridge="82G965" CodeName="Broadwater"/>
    <Chipset Name ="Q965" Northbridge="82Q965" CodeName="Broadwater"/>
    <Chipset Name ="G35" Northbridge="82G35" CodeName="Bearlake"/>
    <Chipset Name ="Q33" Northbridge="82Q33" CodeName="Bearlake"/>
    <Chipset Name ="Q35" Northbridge="82Q35" CodeName="Bearlake"/>
    <Chipset Name ="P43" Northbridge="82P43" CodeName="Eaglelake"/>
    <Chipset Name ="P45" Northbridge="82P45" CodeName="Eaglelake"/>
    <Chipset Name ="G41" Northbridge="82G43" CodeName="Eaglelake"/>
    <Chipset Name ="G43" Northbridge="82G43" CodeName="Eaglelake"/>
    <Chipset Name ="G45" Northbridge="82G45" CodeName="Eaglelake"/>
    <Chipset Name ="B43" Northbridge="82B43" CodeName="Eaglelake"/>
    <Chipset Name ="Q43" Northbridge="82Q43" CodeName="Eaglelake"/>
    <Chipset Name ="Q45" Northbridge="82Q45" CodeName="Eaglelake"/>
    <Chipset Name ="4 Series" Northbridge="" CodeName="Eaglelake"/>
    <Chipset Name ="GL960" Northbridge="82960GL" CodeName="Crestline"/>
    <Chipset Name ="GM965" Northbridge="82960GM" CodeName="Crestline"/>
    <Chipset Name ="PM965" Northbridge="82960PM" CodeName="Crestline"/>
    <Chipset Name ="GL40" Northbridge="82GL40" CodeName="Cantiga"/>
    <Chipset Name ="GS40" Northbridge="82GS40" CodeName="Cantiga"/>
    <Chipset Name ="GS45" Northbridge="82GS45" CodeName="Cantiga"/>
    <Chipset Name ="GM45" Northbridge="82GM45" CodeName="Cantiga"/>
    <Chipset Name ="PM45" Northbridge="82PM45" CodeName="Cantiga"/>
    <Chipset Name ="X58" Northbridge="82X58" CodeName="Tylersburg"/>
    <Chipset Name ="P55" Northbridge="82P55" CodeName="Ibex Peak"/>
    <Chipset Name ="H55" Northbridge="82H55" CodeName="Ibex Peak"/>
    <Chipset Name ="H57" Northbridge="82H57" CodeName="Ibex Peak"/>
    <Chipset Name ="Q57" Northbridge="82Q57" CodeName="Ibex Peak"/>
    <Chipset Name ="H61" Northbridge="82H61" CodeName="Cougar Point"/>
    <Chipset Name ="P67" Northbridge="82P67" CodeName="Cougar Point"/>
    <Chipset Name ="H67" Northbridge="BD82H67" CodeName="Cougar Point"/>
    <Chipset Name ="Z68" Northbridge="82Z68" CodeName="Cougar Point"/>
    <Chipset Name ="Q67" Northbridge="82Q67" CodeName="Cougar Point"/>
    <Chipset Name ="Q65" Northbridge="82Q65" CodeName="Cougar Point"/>
    <Chipset Name ="B65" Northbridge="82B65" CodeName="Cougar Point"/>
    <Chipset Name ="X79" Northbridge="82X79" CodeName="Patsburg"/>
    <Chipset Name ="Z77" Northbridge="82Z77" CodeName="Panther Point"/>
    <Chipset Name ="Z75" Northbridge="82Z75" CodeName="Panther Point"/>
    <Chipset Name ="H77" Northbridge="82H77" CodeName="Panther Point"/>
    <Chipset Name ="Q77" Northbridge="82Q77" CodeName="Panther Point"/>
    <Chipset Name ="Q75" Northbridge="82Q75" CodeName="Panther Point"/>
    <Chipset Name ="B75" Northbridge="82B75" CodeName="Panther Point"/>
    <Chipset Name ="PM55" Northbridge="82PM55" CodeName="Ibex Peak"/>
    <Chipset Name ="HM55" Northbridge="82HM55" CodeName="Ibex Peak"/>
    <Chipset Name ="HM57" Northbridge="82HM57" CodeName="Ibex Peak"/>
    <Chipset Name ="QM57" Northbridge="82QM57" CodeName="Ibex Peak"/>
    <Chipset Name ="QS57" Northbridge="82QS57" CodeName="Ibex Peak"/>
    <Chipset Name ="HM65" Northbridge="82HM65" CodeName="Cougar Point"/>
    <Chipset Name ="HM67" Northbridge="82HM67" CodeName="Cougar Point"/>
    <Chipset Name ="UM67" Northbridge="82UM67" CodeName="Cougar Point"/>
    <Chipset Name ="QM67" Northbridge="82QM67" CodeName="Cougar Point"/>
    <Chipset Name ="QS67" Northbridge="82QS67" CodeName="Cougar Point"/>
    <Chipset Name ="HM75" Northbridge="82HM75" CodeName="Panther Point"/>
    <Chipset Name ="HM76" Northbridge="82HM76" CodeName="Panther Point"/>
    <Chipset Name ="HM77" Northbridge="82HM77" CodeName="Panther Point"/>
    <Chipset Name ="UM77" Northbridge="82UM77" CodeName="Panther Point"/>
    <Chipset Name ="QM77" Northbridge="82QM77" CodeName="Panther Point"/>
    <Chipset Name ="QS77" Northbridge="82QS77" CodeName="Panther Point"/>
    <Chipset Name ="C608" Northbridge="" CodeName="Patsburg"/>
    <Chipset Name ="C600 series" Northbridge="" CodeName="Patsburg"/>
    <Chipset Name ="C606" Northbridge="" CodeName="Patsburg"/>
    <Chipset Name ="C604" Northbridge="" CodeName="Patsburg"/>    
    <Chipset Name ="C602" Northbridge="" CodeName="Patsburg"/>
    <Chipset Name ="C200 Series" Northbridge="" CodeName="Patsburg"/>
    <Chipset Name ="C216" Northbridge="" CodeName="Patsburg"/>
    <Chipset Name ="C202" Northbridge="" CodeName="Patsburg"/>
    <Chipset Name ="C204" Northbridge="" CodeName="Patsburg"/>
    <Chipset Name ="C206" Northbridge="" CodeName="Patsburg"/>
	<Chipset Name ="45 Express" Northbridge="" CodeName=""/>
  </Vendor>

  <Vendor Name ="AMD">
    <Chipset Name ="640" Northbridge="640" CodeName=""/>
    <Chipset Name ="750" Northbridge="751" CodeName=""/>
    <Chipset Name ="760" Northbridge="761" CodeName=""/>
    <Chipset Name ="760MP" Northbridge="762" CodeName=""/>
    <Chipset Name ="760MPX" Northbridge="762" CodeName=""/>
    <Chipset Name ="8000" Northbridge="8111" CodeName=""/>
    <Chipset Name ="480X" Northbridge="RD480" CodeName=""/>
    <Chipset Name ="570X" Northbridge="RD570" CodeName=""/>
    <Chipset Name ="550X" Northbridge="RD570" CodeName=""/>
    <Chipset Name ="580X" Northbridge="RD580" CodeName=""/>
    <Chipset Name ="690V" Northbridge="RS690C" CodeName=""/>
    <Chipset Name ="690G" Northbridge="RS690" CodeName=""/>
    <Chipset Name ="M690V" Northbridge="RS690MC" CodeName=""/>
    <Chipset Name ="M690" Northbridge="RS690M" CodeName=""/>
    <Chipset Name ="M690E" Northbridge="RS690T" CodeName=""/>
    <Chipset Name ="M690T" Northbridge="RS690T" CodeName=""/>
    <Chipset Name ="740" Northbridge="RX740" CodeName=""/>
    <Chipset Name ="740G" Northbridge="RX740" CodeName=""/>
    <Chipset Name ="760G" Northbridge="RS780L" CodeName=""/>
    <Chipset Name ="770" Northbridge="RX780" CodeName=""/>
    <Chipset Name ="780V" Northbridge="RS780C" CodeName=""/>
    <Chipset Name ="780G" Northbridge="RS780I" CodeName=""/>
    <Chipset Name ="M780V" Northbridge="RS780MC" CodeName=""/>
    <Chipset Name ="M780G" Northbridge="RS780M" CodeName=""/>
    <Chipset Name ="785G" Northbridge="RS880" CodeName=""/>
    <Chipset Name ="790GX" Northbridge="RS780D" CodeName=""/>
    <Chipset Name ="790X" Northbridge="RD780" CodeName=""/>
    <Chipset Name ="790FX" Northbridge="RD790" CodeName=""/>
    <Chipset Name ="870" Northbridge="RX880" CodeName=""/>
    <Chipset Name ="880G" Northbridge="RS880P" CodeName=""/>
    <Chipset Name ="880M" Northbridge="RS880M" CodeName=""/>
    <Chipset Name ="890GX" Northbridge="RS880D" CodeName=""/>
    <Chipset Name ="890FX" Northbridge="RD890" CodeName=""/>
    <Chipset Name ="970" Northbridge="RD970" CodeName=""/>
    <Chipset Name ="990X" Northbridge="RD990" CodeName=""/>
    <Chipset Name ="990FX" Northbridge="RD990" CodeName=""/>
        
  </Vendor>
  

  <Vendor Name ="VIA">
    
    <Chipset Name ="" Northbridge="82C575M" CodeName="Apollo Master"/>
    <Chipset Name ="" Northbridge="82C577M" CodeName="Apollo Master"/>
    <Chipset Name ="VP2/97" Northbridge="82C595" CodeName="Apollo"/>
    <Chipset Name ="VP" Northbridge="82C585VP" CodeName="Apollo"/>
    <Chipset Name ="VP2/97" Northbridge="82C595" CodeName="Apollo"/>
    <Chipset Name ="VPX/97" Northbridge="82C595" CodeName="Apollo"/>
    <Chipset Name ="VP3" Northbridge="82C597" CodeName="Apollo "/>
    <Chipset Name ="MVP3" Northbridge="82C598AT" CodeName="Apollo"/>
    <Chipset Name ="MVP4" Northbridge="8501" CodeName="Apollo"/>
    <Chipset Name ="" Northbridge="82C691" CodeName="Apollo Pro"/>
    <Chipset Name ="" Northbridge="62C692BX" CodeName="Apollo Pro"/>
    <Chipset Name ="" Northbridge="82C693" CodeName="Apollo Pro+"/>
    <Chipset Name ="" Northbridge="82C693A" CodeName="Apollo Pro 133"/>
    <Chipset Name ="" Northbridge="82694X" CodeName="Apollo Pro 133A"/>
    <Chipset Name ="" Northbridge="82C694MP" CodeName="Apollo Pro 133A"/>
    <Chipset Name ="" Northbridge="82C694T" CodeName="Apollo Pro 133T"/>
    <Chipset Name ="" Northbridge="8601" CodeName="ProSavage PM133"/>
    <Chipset Name ="" Northbridge="8601A" CodeName="PLE133"/>
    <Chipset Name ="" Northbridge="8601T" CodeName="PLE133T"/>
    <Chipset Name ="" Northbridge="8633" CodeName="Apollo Pro 266"/>
    <Chipset Name ="" Northbridge="8653" CodeName="Apollo Pro 266T"/>
    <Chipset Name ="" Northbridge="8622" CodeName="CLE266"/>
    <Chipset Name ="" Northbridge="8633" CodeName="CLE266"/>
    <Chipset Name ="KX133" Northbridge="8371" CodeName=""/>
    <Chipset Name ="KT133" Northbridge="8363" CodeName=""/>
    <Chipset Name ="KT133A" Northbridge="8363A" CodeName=""/>
    <Chipset Name ="KT266" Northbridge="8366" CodeName=""/>
    <Chipset Name ="KT266A" Northbridge="8366A" CodeName=""/>
    <Chipset Name ="KT333" Northbridge="8367" CodeName=""/>
    <Chipset Name ="KT400" Northbridge="8368" CodeName=""/>
    <Chipset Name ="KT400A" Northbridge="8377A" CodeName=""/>
    <Chipset Name ="KT600" Northbridge="8377" CodeName=""/>
    <Chipset Name ="KT880" Northbridge="8379" CodeName=""/>
    <Chipset Name ="P4M266" Northbridge="8751" CodeName=""/>
    <Chipset Name ="P4X266" Northbridge="8753" CodeName=""/>
    <Chipset Name ="P4X266E" Northbridge="8753" CodeName=""/>
    <Chipset Name ="P4X266A" Northbridge="8753" CodeName=""/>
    <Chipset Name ="P4X333" Northbridge="83754" CodeName=""/>
    <Chipset Name ="P4X400" Northbridge="8754" CodeName=""/>
    <Chipset Name ="P4X533" Northbridge="P4X533" CodeName=""/>
    <Chipset Name ="K8T800" Northbridge="K8T800" CodeName=""/>
    <Chipset Name ="K8M800" Northbridge="K8M800" CodeName=""/>
    <Chipset Name ="K8T800 Pro" Northbridge="K8T800" CodeName=""/>
    <Chipset Name ="K8T890" Northbridge="K8T890" CodeName=""/>
    <Chipset Name ="K8M890" Northbridge="K8M890" CodeName=""/>
    <Chipset Name ="PM800" Northbridge="PM800" CodeName=""/>
    <Chipset Name ="PT800" Northbridge="PT800" CodeName=""/>
    <Chipset Name ="P4M800 Pro" Northbridge="P4M800" CodeName=""/>
    <Chipset Name ="P4M890" Northbridge="P4M890" CodeName=""/>
    <Chipset Name ="PT890" Northbridge="PT890" CodeName=""/>
    <Chipset Name ="P4M900" Northbridge="P4M900" CodeName=""/>
    <Chipset Name ="PM880" Northbridge="PM880" CodeName=""/>
    <Chipset Name ="PT880" Northbridge="PT880" CodeName=""/>
    <Chipset Name ="PT880 Pro" Northbridge="PT880" CodeName=""/>
    <Chipset Name ="PT880 Ultra" Northbridge="PT880" CodeName=""/>
    <Chipset Name ="PT894" Northbridge="PT894" CodeName=""/>
    <Chipset Name ="VX700" Northbridge="VX700" CodeName=""/>
    <Chipset Name ="CN700" Northbridge="CN700" CodeName=""/>
    <Chipset Name ="CN800" Northbridge="CN800" CodeName=""/>
    <Chipset Name ="CN896" Northbridge="CN896" CodeName=""/>
    <Chipset Name ="VX800" Northbridge="VX800" CodeName=""/>
    <Chipset Name ="VX855" Northbridge="VX855" CodeName=""/>
    <Chipset Name ="VX900" Northbridge="VX900" CodeName=""/>
    <Chipset Name ="VN1000" Northbridge="VN1000" CodeName=""/>
  </Vendor>
 
  
  
  
  <Vendor Name ="SiS">
    <Chipset Name ="496" Northbridge="85C496" CodeName=""/>
    <Chipset Name ="5598" Northbridge="5598" CodeName=""/>
    <Chipset Name ="530" Northbridge="530" CodeName=""/>
    <Chipset Name ="620" Northbridge="620" CodeName=""/>
    <Chipset Name ="620" Northbridge="620" CodeName=""/>
    <Chipset Name ="630" Northbridge="630" CodeName=""/>
    <Chipset Name ="630E" Northbridge="630E" CodeName=""/>
    <Chipset Name ="630ET" Northbridge="630ET" CodeName=""/>
    <Chipset Name ="630S" Northbridge="630S" CodeName=""/>
    <Chipset Name ="630ST" Northbridge="630ST" CodeName=""/>
    <Chipset Name ="645" Northbridge="645" CodeName=""/>
    <Chipset Name ="645DX" Northbridge="645DX" CodeName=""/>
    <Chipset Name ="648" Northbridge="648" CodeName=""/>
    <Chipset Name ="648FX" Northbridge="648FX" CodeName=""/>
    <Chipset Name ="649" Northbridge="649" CodeName=""/>
    <Chipset Name ="641FX" Northbridge="641FX" CodeName=""/>
    <Chipset Name ="650" Northbridge="650" CodeName=""/>
    <Chipset Name ="651" Northbridge="651" CodeName=""/>
    <Chipset Name ="655" Northbridge="655" CodeName=""/>
    <Chipset Name ="655FX" Northbridge="655FX" CodeName=""/>
    <Chipset Name ="655TX" Northbridge="655TX" CodeName=""/>
    <Chipset Name ="656" Northbridge="656" CodeName=""/>
    <Chipset Name ="656FX" Northbridge="656FX" CodeName=""/>
    <Chipset Name ="R658" Northbridge="R658" CodeName=""/>
    <Chipset Name ="R659" Northbridge="R659" CodeName=""/>
    <Chipset Name ="660FX" Northbridge="660FX" CodeName=""/>
    <Chipset Name ="661FX" Northbridge="661FX" CodeName=""/>
    <Chipset Name ="662" Northbridge="662" CodeName=""/>
    <Chipset Name ="670" Northbridge="670" CodeName=""/>
    <Chipset Name ="730S" Northbridge="730S" CodeName=""/>
    <Chipset Name ="733" Northbridge="733" CodeName=""/>
    <Chipset Name ="735" Northbridge="735" CodeName=""/>
    <Chipset Name ="740" Northbridge="740" CodeName=""/>
    <Chipset Name ="741" Northbridge="741" CodeName=""/>
    <Chipset Name ="745" Northbridge="745" CodeName=""/>
    <Chipset Name ="746" Northbridge="746" CodeName=""/>
    <Chipset Name ="746FX" Northbridge="746FX" CodeName=""/>
    <Chipset Name ="746" Northbridge="746" CodeName=""/>
    <Chipset Name ="750" Northbridge="750" CodeName=""/>
    <Chipset Name ="755FX" Northbridge="755FX" CodeName=""/>
    <Chipset Name ="760" Northbridge="760" CodeName=""/>
    <Chipset Name ="760GX" Northbridge="760GX" CodeName=""/>
    <Chipset Name ="761" Northbridge="761" CodeName=""/>
    <Chipset Name ="761GL" Northbridge="761GL" CodeName=""/>
    <Chipset Name ="761GX" Northbridge="761GX" CodeName=""/>
    <Chipset Name ="770" Northbridge="770" CodeName=""/>   
  
  </Vendor> 
  
  <Vendor Name ="ULi">
    <Chipset Name ="M1695" Northbridge="" CodeName=""/>
    <Chipset Name ="M1697" Northbridge="" CodeName=""/>
    <Chipset Name ="M1689" Northbridge="" CodeName=""/>  
  </Vendor>
  
  
  <Vendor Name ="ALi">
    <Chipset Name ="1217" Northbridge="" CodeName=""/>    
    <Chipset Name ="1429" Northbridge="" CodeName=""/>
    <Chipset Name ="1439" Northbridge="" CodeName=""/>
    <Chipset Name ="1431" Northbridge="" CodeName=""/>
    <Chipset Name ="1445" Northbridge="" CodeName=""/>
    <Chipset Name ="1489" Northbridge="" CodeName=""/>
    <Chipset Name ="1521" Northbridge="" CodeName="ALADDiN III"/>
    <Chipset Name ="1531" Northbridge="" CodeName="ALADDiN IV"/>
    <Chipset Name ="1543" Northbridge="" CodeName="ALADDiN IV"/>
    <Chipset Name ="1531B" Northbridge="" CodeName="ALADDiN IV+"/>
    <Chipset Name ="1541" Northbridge="" CodeName="ALADDiN V"/>
    <Chipset Name ="1542" Northbridge="" CodeName="ALADDiN V"/>
    <Chipset Name ="1561" Northbridge="" CodeName="ALADDiN 7"/>
    <Chipset Name ="1535D" Northbridge="" CodeName="ALADDiN 7"/>
    <Chipset Name ="1621" Northbridge="" CodeName="ALADDiN-Pro II"/>
    <Chipset Name ="1543" Northbridge="" CodeName="ALADDiN-Pro II"/>
    <Chipset Name ="1631" Northbridge="" CodeName="ALADDiN TNT2"/>
    <Chipset Name ="1632M" Northbridge="" CodeName="CyberBLADE ALADDiN i1"/>
    <Chipset Name ="1641B" Northbridge="" CodeName="ALADDiN-Pro 4"/>
    <Chipset Name ="1535D" Northbridge="" CodeName="ALADDiN-Pro 4"/>
    <Chipset Name ="1644" Northbridge="" CodeName="CyberALADDiN"/>
    <Chipset Name ="1644T" Northbridge="" CodeName="CyberALADDiN-T"/>
    <Chipset Name ="1651T" Northbridge="" CodeName="ALADDiN-Pro 5"/>
    <Chipset Name ="1535D+" Northbridge="" CodeName="Aladdin Pro 5T"/>
    <Chipset Name ="1671" Northbridge="" CodeName="ALADDiN-P4"/>
    <Chipset Name ="1672" Northbridge="" CodeName="CyberALADDiN-P4"/>
    <Chipset Name ="1681" Northbridge="" CodeName=""/>
    <Chipset Name ="1683" Northbridge="" CodeName=""/>
    <Chipset Name ="1646" Northbridge="" CodeName="CyberMAGiK"/>
    <Chipset Name ="1647" Northbridge="" CodeName="MobileMAGiK 1"/>
    <Chipset Name ="1647" Northbridge="" CodeName="MAGiK 1"/>
    <Chipset Name ="1535D+" Northbridge="" CodeName="MAGiK 1"/>
    <Chipset Name ="1667" Northbridge="" CodeName="MAGiK 2"/>
    <Chipset Name ="1668" Northbridge="" CodeName="MAGiK K8"/>
    <Chipset Name ="1687" Northbridge="" CodeName="Hammer"/>
    <Chipset Name ="1565" Northbridge="" CodeName="Hammer"/>
    <Chipset Name ="1688" Northbridge="" CodeName="MAGiK K8G"/>
    <Chipset Name ="1688" Northbridge="" CodeName="MAGiK K8G"/>
    <Chipset Name ="1689" Northbridge="" CodeName=""/>
    <Chipset Name ="1565" Northbridge="" CodeName=""/>
    <Chipset Name ="1691" Northbridge="" CodeName=""/>
    <Chipset Name ="1691" Northbridge="" CodeName=""/>
    <Chipset Name ="1673" Northbridge="" CodeName=""/>
    <Chipset Name ="1695" Northbridge="" CodeName=""/>
    <Chipset Name ="1697" Northbridge="" CodeName=""/>      
  </Vendor>
  
  <Vendor Name ="ATI">
    <Chipset Name ="" Northbridge="IGP 320" CodeName="ATI A3"/>
    <Chipset Name ="" Northbridge="IGP 320M" CodeName="ATI U1"/>
    <Chipset Name ="IGP 320M" Northbridge="RS380" CodeName="RS380"/>
    <Chipset Name ="" Northbridge="RX380" CodeName="RX380"/>
    <Chipset Name ="Radeon 380M IGP" Northbridge="RS380M" CodeName="RS380M"/>
    <Chipset Name ="Radeon Xpress 200" Northbridge="RS480" CodeName="RS480"/>
    <Chipset Name ="Radeon Xpress 200P" Northbridge="RX480" CodeName="RX480"/>
    <Chipset Name ="Radeon Xpress 200M" Northbridge="RS480M" CodeName="RS480M"/>
    <Chipset Name ="Radeon Xpress 1100" Northbridge="RS482" CodeName="RS482"/>
    <Chipset Name ="Radeon Xpress 200" Northbridge="RS482" CodeName="RS482"/>
    <Chipset Name ="Radeon Xpress 200" Northbridge="RS485" CodeName="RS485"/>
    <Chipset Name ="Radeon Xpress 1150" Northbridge="RS485" CodeName="RS485"/>
    <Chipset Name ="CrossFire Xpress 1600" Northbridge="RD480" CodeName="RD480"/>
    <Chipset Name ="Radeon Xpress 200 Crossfire Edition" Northbridge="RD480" CodeName="RD480"/>
    <Chipset Name ="Radeon Xpress 3200" Northbridge="RD580" CodeName="RD580"/>
    <Chipset Name ="IGP 330" Northbridge="RS200L" CodeName="RS200L"/>
    <Chipset Name ="IGP 340" Northbridge="RS200" CodeName="RS200"/>
    <Chipset Name ="IGP 340M" Northbridge="RS200M revA" CodeName="RS200M revA"/>
    <Chipset Name ="IGP 345M" Northbridge="RS200M+" CodeName="RS200M+"/>
    <Chipset Name ="IGP 350M" Northbridge="RS200M revB" CodeName="RS200M revB"/>
    <Chipset Name ="IGP 7000" Northbridge="RS250M" CodeName="RS250M"/>    
    <Chipset Name ="IGP 360" Northbridge="RS250M" CodeName="RS250M"/>
    <Chipset Name ="Radeon 9100 PRO IGP" Northbridge="RS350" CodeName="RS350"/>
    <Chipset Name ="Radeon 9000 PRO IGP" Northbridge="RC350" CodeName="RC350"/>
    <Chipset Name ="" Northbridge="RX330" CodeName="RX330"/>
    <Chipset Name ="" Northbridge="RX320" CodeName="RX320"/>
    <Chipset Name ="Radeon Xpress 1150" Northbridge="RS400" CodeName="RS400"/>
    <Chipset Name ="Radeon Xpress 1150" Northbridge="RS400" CodeName="RS415"/>
    <Chipset Name ="Radeon Xpress 1150" Northbridge="RC400" CodeName="RC400"/>
    <Chipset Name ="Radeon Xpress 1150" Northbridge="RS410" CodeName="RS410"/>
    <Chipset Name ="Radeon Xpress 1250" Northbridge="RS600" CodeName="RS600"/>
    <Chipset Name ="Radeon CrossFire Xpress 3200" Northbridge="RD600" CodeName="RD600"/>  
     
  </Vendor>

  <Vendor Name ="NVIDIA">    
    <Chipset Name ="900 Series" Northbridge="" CodeName="nForce"/> 
    <Chipset Name ="700 Series" Northbridge="" CodeName="nForce"/> 
    <Chipset Name ="600 Series" Northbridge="" CodeName="nForce"/> 
    <Chipset Name ="500 Series" Northbridge="" CodeName="nForce"/> 
    <Chipset Name ="4 Series" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="3 Series" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="2 Series" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="nForce Series" Northbridge="" CodeName=""/>
    <Chipset Name ="220" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="415" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="420" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="400" Northbridge="" CodeName="nForce2"/>
    <Chipset Name ="Ultra 400" Northbridge="" CodeName="nForce2"/>
    <Chipset Name ="250" Northbridge="" CodeName="nForce3"/>
    <Chipset Name ="SLI X16" Northbridge="" CodeName="nForce4"/>
    <Chipset Name ="SLI XE" Northbridge="" CodeName="nForce4"/>
    <Chipset Name ="630a" Northbridge="GeForce 7025" CodeName="nForce"/>
    <Chipset Name ="630a" Northbridge="GeForce 7050" CodeName="nForce"/>
    <Chipset Name ="630a" Northbridge="GeForce 7050PV" CodeName="nForce"/>
    <Chipset Name ="630a SLI" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="610i" Northbridge="GeForce 7050" CodeName="nForce"/>
    <Chipset Name ="630i" Northbridge="GeForce 7100" CodeName="nForce"/>
    <Chipset Name ="650i" Northbridge="GeForce 7150" CodeName="nForce"/>
    <Chipset Name ="650i Ultra" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="650i SLI" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="680i LT SLI" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="680i SLI" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="720a" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="720d" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="730a" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="750a" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="780a" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="730i" Northbridge="Geforce 9300" CodeName="nForce"/>
    <Chipset Name ="750i SLI" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="780i SLI" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="790i SLI" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="980a" Northbridge="" CodeName="nForce"/>
    <Chipset Name ="150" Northbridge="" CodeName="nForce3 Professional"/>
    <Chipset Name ="250" Northbridge="" CodeName="nForce3 Professional"/>
    <Chipset Name ="2200 MCP" Northbridge="" CodeName="nForce3 Professional"/>
    <Chipset Name ="2050 MCP" Northbridge="" CodeName="nForce3 Professional"/>
    <Chipset Name ="3400 MCP" Northbridge="" CodeName="nForce3 Professional"/>
    <Chipset Name ="3600 MCP" Northbridge="" CodeName="nForce3 Professional"/>
    <Chipset Name ="3050 MCP" Northbridge="" CodeName="nForce3 Professional"/>
  </Vendor>
</ChipseInfo>';

     exec dbo.AddChipsetLibrary  @XMLObject;


END TRY
BEGIN CATCH

     
     if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка при добавлении библиотеки чипсетов';
	 else
	   set @Message = Error_Message();
	   
     RAISERROR(@Message, 12, 1);
     Return -1;

END CATCH

	
END


