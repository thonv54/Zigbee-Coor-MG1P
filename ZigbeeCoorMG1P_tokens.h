// This file is generated by Simplicity Studio.  Please do not edit manually.
//
//

// This file contains the tokens for attributes stored in flash


// Identifier tags for tokens
// Creator for attribute: application version, singleton.
#define CREATOR_APPLICATION_VERSION_SINGLETON 0xB000
// Creator for attribute: stack version, singleton.
#define CREATOR_STACK_VERSION_SINGLETON 0xB001
// Creator for attribute: hardware version, singleton.
#define CREATOR_HW_VERSION_SINGLETON 0xB002


// Types for the tokens
#ifdef DEFINETYPES
typedef uint8_t  tokType_application_version;
typedef uint8_t  tokType_stack_version;
typedef uint8_t  tokType_hw_version;
#endif // DEFINETYPES


// Actual token definitions
#ifdef DEFINETOKENS
DEFINE_BASIC_TOKEN(APPLICATION_VERSION_SINGLETON, tokType_application_version, 1)
DEFINE_BASIC_TOKEN(STACK_VERSION_SINGLETON, tokType_stack_version, 8)
DEFINE_BASIC_TOKEN(HW_VERSION_SINGLETON, tokType_hw_version, 1)
#endif // DEFINETOKENS


// Macro snippet that loads all the attributes from tokens
#define GENERATED_TOKEN_LOADER(endpoint) do {\
  uint8_t ptr[1]; \
  uint8_t curNetwork = emberGetCurrentNetwork(); \
  uint8_t epNetwork; \
  halCommonGetToken((tokType_application_version *)ptr, TOKEN_APPLICATION_VERSION_SINGLETON); \
  emberAfWriteServerAttribute(1, ZCL_BASIC_CLUSTER_ID, ZCL_APPLICATION_VERSION_ATTRIBUTE_ID, (uint8_t*)ptr, ZCL_INT8U_ATTRIBUTE_TYPE); \
  halCommonGetToken((tokType_stack_version *)ptr, TOKEN_STACK_VERSION_SINGLETON); \
  emberAfWriteServerAttribute(1, ZCL_BASIC_CLUSTER_ID, ZCL_STACK_VERSION_ATTRIBUTE_ID, (uint8_t*)ptr, ZCL_INT8U_ATTRIBUTE_TYPE); \
  halCommonGetToken((tokType_hw_version *)ptr, TOKEN_HW_VERSION_SINGLETON); \
  emberAfWriteServerAttribute(1, ZCL_BASIC_CLUSTER_ID, ZCL_HW_VERSION_ATTRIBUTE_ID, (uint8_t*)ptr, ZCL_INT8U_ATTRIBUTE_TYPE); \
} while(false)


// Macro snippet that saves the attribute to token
#define GENERATED_TOKEN_SAVER do {\
  uint8_t allZeroData[1]; \
  MEMSET(allZeroData, 0, 1); \
  if ( data == NULL ) data = allZeroData; \
  if ( clusterId == 0x00 ) { \
    if ( metadata->attributeId == 0x0001 && !emberAfAttributeIsClient(metadata) ) \
      halCommonSetToken(TOKEN_APPLICATION_VERSION_SINGLETON, data); \
    if ( metadata->attributeId == 0x0002 && !emberAfAttributeIsClient(metadata) ) \
      halCommonSetToken(TOKEN_STACK_VERSION_SINGLETON, data); \
    if ( metadata->attributeId == 0x0003 && !emberAfAttributeIsClient(metadata) ) \
      halCommonSetToken(TOKEN_HW_VERSION_SINGLETON, data); \
  }\
} while(false)


