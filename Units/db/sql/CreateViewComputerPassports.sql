
CREATE VIEW ViewComputerPassport
AS
SELECT    'Passport' as ObjectType, P.Name, P.ObjectGuid,   P.Description,  
          P.ParentGUID as PassportContainerGUID,  L.ParentGuid,   L.ObjectGuid as LinkGuid
FROM    PassportLink L, Passport P 
WHERE   P.ObjectGuid = L.PassportGuid;



