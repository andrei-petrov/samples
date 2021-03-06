CREATE PROCEDURE [dbo].[InsertInitialData] 

AS
BEGIN
	
	SET NOCOUNT ON;

BEGIN TRY

declare @Message  nvarchar(350);
declare @Check smallint;

BEGIN TRANSACTION
   
  
   set @Check = 0;
   
   insert into dbo.Localization values('Африканский', '1078'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Албанский', '1052'); 
   if (@@Error <> 0)
   set @Check = -1; 
   
   insert into dbo.Localization values('Амхарский (Эфиопия)', '1118'); 
   if (@@Error <> 0)
   set @Check = -1; 
   
   insert into dbo.Localization values('Арабский (Алжир)', '5121'); 
   if (@@Error <> 0)
   set @Check = -1; 
    
   insert into dbo.Localization values('Арабский (Бахрейн)', '15361'); 
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Localization values('Арабский (Египт)', '3073'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Арабский (Ирак)', '2049'); 
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Localization values('Арабский (Иордания)', '11265'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Арабский (Кювейт)', '13313'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Арабский (Ливан)', '12289'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Арабский (Ливия)', '4097'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Арабский (Марокко)', '6145'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Арабский (Оман)', '8193'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Арабский (Катар)', '16385'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Арабский (Саудовская Аравия)', '1025'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Арабский (Сирия)', '10241'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Арабский (Тунис)', '7169'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Арабский (О.А.Э.)', '14337'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Арабский (Йемен)', '9217'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Армянский', '1067'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Ассамский', '1101'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Азербайджанский (Кириллица)', '2092'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Азербайджанский (Латиница)', '1068'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Баскский', '1069'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Беларусский', '1059'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Бенгали (Бангладеш)', '2117'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Бенгали (Индия)', '1093'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Боснийский (Босния\Герциговина)', '5146'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Болгарский', '1026'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Бирманский', '1109'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Каталонский', '1027'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Китайский (Гонконг)', '3076'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Китайский (Макау)', '5124'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Китайский (КНР)', '2052'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Китайский (Сингапур)', '4100'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Китайский (Тайвань)', '1028'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Хорватский', '1050'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Хорватский (Босния\Герциговина)', '4122'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Чешский', '1029'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Датский', '1030'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Дивеи', '1125'); 
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Localization values('Голландский (Бельгия)', '2067'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Голландский (Нидерланды)', '1043'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Эдо', '1126'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Английский (Австралия)', '3081'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Английский (Белиз)', '10249'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Английский (Канада)', '4105'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Английский (Карибские острова)', '9225'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Английский (Индия)', '16393'); 
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Localization values('Английский (Ирландия)', '6153'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Английский (Ямайка)', '8201'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Английский (Новозеландия)', '5129'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Английский (Филиппины)', '13321'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Английский (Южная Африка)', '7177'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Английский (Тринидад)', '11273'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Английский (Объединенное королевство)', '2057'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Английский (США)', '1033'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Английский (Зимбабве)', '12297'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Эстонский', '1061'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Фарерский', '1080'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Фарский', '1065'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Филиппинский', '1124'); 
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Localization values('Финский', '1035'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Французский (Бельгия)', '2060'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Французский (Камерун)', '11276'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Французский (Канада)', '3084'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Французский (Конго)', '9228'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Французский (Кот Д''Ивуар)', '12300'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Французский (Франция)', '1036'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Французский (Люксембург)', '5132'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Французский (Мали)', '13324'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Французский (Монако)', '6156'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Французский (Morocco)', '14348'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Французский (Сенегал)', '10252'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Французский (Швейцария)', '4108'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Французский (Западная Индия)', '7180'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Фризский (Нидерланды)', '1122'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Македонский', '1071'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Гэльский Ирландия', '2108'); 
   if (@@Error <> 0)
   set @Check = -1;
        
   insert into dbo.Localization values('Гэльский Шотландия', '1084'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Галисийский', '1110'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Грузия', '1079'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Немецкий (Австрия)', '3079'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Немецкий (Германия)', '1031'); 
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Localization values('Немецкий (Лихтенштейн)', '5127'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Немецкий (Люксембург)', '4103'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Немецкий (Швейцария)', '2055'); 
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Localization values('Греческий', '1032'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Гуарани (Paraguay)', '1140'); 
   if (@@Error <> 0)
   set @Check = -1;
        
   insert into dbo.Localization values('Гуярати', '1095'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Иврит', '1037'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('HID (Human Interface Device)', '1279'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Хинди', '1081'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Венгерский', '1038'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Исландский', '1039'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Игбо (Нигерия)', '1136'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Индонезийский', '1057'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Итальянский (Италия)', '1040'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Итальянский (Швейцария)', '2064'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Японский', '1041'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Каннада', '1099'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Кашмирский', '1120'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Казахский', '1087'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Кмерский', '1107'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Конкани', '1111'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Корейский', '1042'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Киргизский (Кириллица)', '1088'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Лаосский', '1108'); 
   if (@@Error <> 0)
   set @Check = -1;
        
   insert into dbo.Localization values('Латинский', '1142'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Латвийский', '1062'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Литовский', '1063'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Малайский (Бруней)', '2110'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Малайский (Малазия)', '1086'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Малаялам', '1100'); 
   if (@@Error <> 0)
   set @Check = -1;
        
   insert into dbo.Localization values('Мальтийский', '1082'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Манипури', '1112'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Маори (Новозеландия)', '1153'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Маратхи', '1102'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Монгольский (Кириллица)', '1104'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Монгольский (Монголия)', '2128'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Непальский', '1121'); 
   if (@@Error <> 0)
   set @Check = -1;
        
   insert into dbo.Localization values(' ', '0'); 
   if (@@Error <> 0)
   set @Check = -1;
        
   insert into dbo.Localization values('Норвежский (Бокмал)', '1044'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Норвежский (Нинорск)', '2068'); 
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Localization values('Ория', '1096'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Польский', '1045'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Португальский (Бразилия)', '1046'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Португальский (Португалия)', '2070'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Панджаби', '1094'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Ретороманский', '1047'); 
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Localization values('Румынский', '1048'); 
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Localization values('Румынский (Молдавия)', '2072'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Русский', '1049'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Русский (Молдавия)', '2073'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Саамский', '1083'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Санскрит', '1103'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Сербский (Кириллица)', '3098'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Сербский (Латиница)', '2074'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Sesotho', '1072'); 
   if (@@Error <> 0)
   set @Check = -1;
        
   insert into dbo.Localization values('Синдхи', '1113'); 
   if (@@Error <> 0)
   set @Check = -1;
        
   insert into dbo.Localization values('Сингальский (Шри Ланка)', '1115'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Словацкий', '1051'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Словенский', '1060'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Сомалийский', '1143'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Сорбский', '1070'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Испанский (Аргентина)', '11274'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Боливия)', '16394'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Чили)', '13322'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Колумбия)', '9226'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Коста Рика)', '5130'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Доминиканская республика)', '7178'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Эквадор)', '12298'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Сальвадор)', '17418'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Гватемала)', '4106'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Гондурас)', '18442'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Смешанный)', '3082'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Мексика)', '2058'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Никарагуа)', '19466'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Панама)', '6154'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Парагвай)', '15370'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Перу)', '10250'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Пуерто Рико)', '20490'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Традиционный)', '1034'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Уругвай)', '14346'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Испанский (Венесуэла)', '8202'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Суту', '1072'); 
   if (@@Error <> 0)
   set @Check = -1;
        
   insert into dbo.Localization values('Суахили', '1089'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Шведский', '1053'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Шведский (Финляндия)', '2077'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Сирийский', '1114'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Тайский', '1064'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Тамильский', '1097'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Татарский', '1092'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Телугу', '1098'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Тайский', '1054'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Тибетский', '1105'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Тсонга', '1073'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Тсвана', '1074'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Турецкий', '1055'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Туркменский', '1090'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Украинский', '1058'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Урду', '1056'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Узбекский (Кириллица)', '2115'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Узбекский (Латиница)', '1091'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Венда', '1075'); 
   if (@@Error <> 0)
   set @Check = -1;
       
   insert into dbo.Localization values('Вьетнамский', '1066'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Уэльский', '1106'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Хоса', '1076'); 
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Localization values('Идиш', '1085'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Зулу', '1077'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Русский', 'ru_RU'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Английский (США)', 'en_US'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Английский', '9'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Испанский', '10'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Французский', '12'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Итальянский', '16'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Немецкий', '19'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Немецкий', '7'); 
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Localization values('Русский', '25'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Украинский', '34'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Беларусский', '35'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Беларусский', 'be'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Английский', 'en'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Английский (Объединенное Королевство)', 'en-gb'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Английский (Канада)', 'en-ca'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Французский', 'fr'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Французский (Канада)', 'fr-ca'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Немецкий', 'de'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Немецкий (Австрия)', 'de-at'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Немецкий (Швейцария)', 'de-ch'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Испанский', 'es'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Испанский (Мексика)', 'es-mx'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Испанский (Аргентина)', 'es-ar'); 
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Localization values('Украинский', 'uk'); 
   if (@@Error <> 0)
   set @Check = -1;
   
  
   insert into dbo.InspectionTypeLU values(0, 'Удаленное сканирование реестра');
   if (@@Error <> 0)
   set @Check = -1;

   insert into dbo.InspectionTypeLU values(1, 'Удаленный доступ с помощью агента');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.InspectionTypeLU values(2, 'Импорт данных от автономного агента');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.InspectionTypeLU values(3, 'Синхронизация задания с активной директорией');
   if (@@Error <> 0)
   set @Check = -1;

   /*insert into dbo.ActionLU values(1, 'Deploy');
   if (@@Error <> 0)
   set @Check = -1;

   insert into dbo.ActionLU values(2, 'CheckShare');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.ActionLU values(4, 'Inspect');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.ActionLU values(8, 'Uninstall');
   if (@@Error <> 0)
   set @Check = -1; */
   
   insert into dbo.ResultCodeLU values(0, 'Неуспешно');
   if (@@Error <> 0)
   set @Check = -1;

   insert into dbo.ResultCodeLU values(1, 'Успешно');
   if (@@Error <> 0)
   set @Check = -1;
   
   
   insert into dbo.SoftTypeLU values(0, 'Программное обеспечение', 1);
   if (@@Error <> 0)
   set @Check = -1;

   insert into dbo.SoftTypeLU values(1, 'Операционные системы', 0);
   if (@@Error <> 0)
   set @Check = -1;
   
   
   insert into dbo.InspectionListParamLU values(0, 'Все инспекции', 0, 1);
   if (@@Error <> 0)
   set @Check = -1;

   insert into dbo.InspectionListParamLU values(1, 'Успешные', 1, 1);
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.InspectionListParamLU values(2, 'Неуспешные', 0, 0);
   if (@@Error <> 0)
   set @Check = -1;
   
   
   
   insert into dbo.StatusLU values(0, 'Unknown');
   if (@@Error <> 0)
   set @Check = -1;

   insert into dbo.StatusLU values(1, 'ShareOpened');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.StatusLU values(2, 'AgentDeployed');
   if (@@Error <> 0)
   set @Check = -1;

   insert into dbo.StatusLU values(3, 'Inspected');
   if (@@Error <> 0)
   set @Check = -1;
   
   
   insert into dbo.ActionTypeLU values(0, 'AgentInstallation');
   if (@@Error <> 0)
   set @Check = -1;

   insert into dbo.ActionTypeLU values(1, 'Inspection');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.ActionTypeLU values(2, 'AgentRemoving');
   if (@@Error <> 0)
   set @Check = -1;
   
   
   
   insert into dbo.SecurityTypeLU values(0, 'Антивирус');
   if (@@Error <> 0)
   set @Check = -1;

   insert into dbo.SecurityTypeLU values(1, 'Персональный межсетевой экран');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.SecurityTypeLU values(2, 'СЗИ от НСД');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.SecurityTypeLU values(3, 'АПМДЗ');
   if (@@Error <> 0)
   set @Check = -1;
   
   
   ------  Product list for language cutting from Product name ---------
   
   insert into dbo.ProductList values(0, '%dictionar%');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.ProductList values(1, '%languag%');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.ProductList values(2, '%translat%');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.ProductList values(3, '%interpret%');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.ProductList values(4, '%glossar%');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.ProductList values(5, '%vocabular%');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.ProductList values(6, '%phrasebook%');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.ProductList values(7, '%словар%');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.ProductList values(8, '%перевод%');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.ProductList values(9, '%разговорн%');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.ProductList values(10, '%язык%');
   if (@@Error <> 0)
   set @Check = -1;

   
    ------  Language description list for language cutting from Product name ---------
    
   insert into dbo.Languages values(0, 'Russian');
   if (@@Error <> 0)
   set @Check = -1;

   insert into dbo.Languages values(1, 'English');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(2, 'Spanish');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(3, 'French');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(4, 'Franзais');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(5, 'Deutsch');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(6, 'Ukrainian');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(7, 'рус.');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(8, ' ENU ');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(9, ' RUS ');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(10, 'Русский');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(11, 'German');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(12, ' en ');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(13, ' es ');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(14, ' uk ');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(15, ' fr ');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(16, ' ENU,');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(17, ' RUS,');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(18, ' en,');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(19, ' es,');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(20, ' uk,');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(21, ' fr,');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(22, ' de ');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(23, ' de, ');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(24, ' /de ');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(22, ' be ');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(23, ' be, ');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(24, ' /be ');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(25, 'italian');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(26, 'Belarusian');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(27, ' ru ');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(28, ' ru,');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Languages values(29, ' /ru ');
   if (@@Error <> 0)
   set @Check = -1;
   


   if (@Check = 0)
        COMMIT TRANSACTION
   else 
   begin 
        RAISERROR('Транзакция отменена', 12, 1);
   end

END TRY
BEGIN CATCH

if (Error_Message()  is NULL or Error_Message() = '')
   set @Message = 'Ошибка БД при добавлении начальных данных';
else
   set @Message = Error_Message();
   
ROLLBACK TRANSACTION
RAISERROR(@Message, 12, 1);
Return -1;

END CATCH

END


