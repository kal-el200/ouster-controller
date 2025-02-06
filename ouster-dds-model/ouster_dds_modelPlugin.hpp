

/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from ouster_dds_model.idl
using RTI Code Generator (rtiddsgen) version 4.3.0.
The rtiddsgen tool is part of the RTI Connext DDS distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the Code Generator User's Manual.
*/

#ifndef ouster_dds_modelPlugin_1522706435_h
#define ouster_dds_modelPlugin_1522706435_h

#include "ouster_dds_model.hpp"

struct RTICdrStream;

#ifndef pres_typePlugin_h
#include "pres/pres_typePlugin.h"
#endif

#if (defined(RTI_WIN32) || defined (RTI_WINCE) || defined(RTI_INTIME)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport __declspec(dllexport)
#endif

namespace Ouster {

    #define OusterRayDataPlugin_get_sample PRESTypePluginDefaultEndpointData_getSample

    #define OusterRayDataPlugin_get_buffer PRESTypePluginDefaultEndpointData_getBuffer 
    #define OusterRayDataPlugin_return_buffer PRESTypePluginDefaultEndpointData_returnBuffer

    #define OusterRayDataPlugin_create_sample PRESTypePluginDefaultEndpointData_createSample 
    #define OusterRayDataPlugin_destroy_sample PRESTypePluginDefaultEndpointData_deleteSample 

    /* --------------------------------------------------------------------------------------
    Support functions:
    * -------------------------------------------------------------------------------------- */

    NDDSUSERDllExport extern OusterRayData*
    OusterRayDataPluginSupport_create_data_w_params(
        const struct DDS_TypeAllocationParams_t * alloc_params);

    NDDSUSERDllExport extern OusterRayData*
    OusterRayDataPluginSupport_create_data_ex(RTIBool allocate_pointers);

    NDDSUSERDllExport extern OusterRayData*
    OusterRayDataPluginSupport_create_data(void);

    NDDSUSERDllExport extern RTIBool 
    OusterRayDataPluginSupport_copy_data(
        OusterRayData *out,
        const OusterRayData *in);

    NDDSUSERDllExport extern void 
    OusterRayDataPluginSupport_destroy_data_w_params(
        OusterRayData *sample,
        const struct DDS_TypeDeallocationParams_t * dealloc_params);

    NDDSUSERDllExport extern void 
    OusterRayDataPluginSupport_destroy_data_ex(
        OusterRayData *sample,RTIBool deallocate_pointers);

    NDDSUSERDllExport extern void 
    OusterRayDataPluginSupport_destroy_data(
        OusterRayData *sample);

    NDDSUSERDllExport extern void 
    OusterRayDataPluginSupport_print_data(
        const OusterRayData *sample,
        const char *desc,
        unsigned int indent);

    /* ----------------------------------------------------------------------------
    Callback functions:
    * ---------------------------------------------------------------------------- */

    NDDSUSERDllExport extern RTIBool 
    OusterRayDataPlugin_copy_sample(
        PRESTypePluginEndpointData endpoint_data,
        OusterRayData *out,
        const OusterRayData *in);

    /* ----------------------------------------------------------------------------
    (De)Serialize functions:
    * ------------------------------------------------------------------------- */

    NDDSUSERDllExport extern RTIBool
    OusterRayDataPlugin_serialize_to_cdr_buffer(
        char * buffer,
        unsigned int * length,
        const OusterRayData *sample,
        ::dds::core::policy::DataRepresentationId representation
        = ::dds::core::policy::DataRepresentation::xcdr()); 

    NDDSUSERDllExport extern RTIBool 
    OusterRayDataPlugin_deserialize(
        PRESTypePluginEndpointData endpoint_data,
        OusterRayData **sample, 
        RTIBool * drop_sample,
        struct RTICdrStream *cdrStream,
        RTIBool deserialize_encapsulation,
        RTIBool deserialize_sample, 
        void *endpoint_plugin_qos);

    NDDSUSERDllExport extern RTIBool
    OusterRayDataPlugin_deserialize_from_cdr_buffer(
        OusterRayData *sample,
        const char * buffer,
        unsigned int length);    

    NDDSUSERDllExport extern unsigned int 
    OusterRayDataPlugin_get_serialized_sample_max_size(
        PRESTypePluginEndpointData endpoint_data,
        RTIBool include_encapsulation,
        RTIEncapsulationId encapsulation_id,
        unsigned int current_alignment);

    /* --------------------------------------------------------------------------------------
    Key Management functions:
    * -------------------------------------------------------------------------------------- */
    NDDSUSERDllExport extern PRESTypePluginKeyKind 
    OusterRayDataPlugin_get_key_kind(void);

    NDDSUSERDllExport extern unsigned int 
    OusterRayDataPlugin_get_serialized_key_max_size(
        PRESTypePluginEndpointData endpoint_data,
        RTIBool include_encapsulation,
        RTIEncapsulationId encapsulation_id,
        unsigned int current_alignment);

    NDDSUSERDllExport extern unsigned int 
    OusterRayDataPlugin_get_serialized_key_max_size_for_keyhash(
        PRESTypePluginEndpointData endpoint_data,
        RTIEncapsulationId encapsulation_id,
        unsigned int current_alignment);

    NDDSUSERDllExport extern RTIBool 
    OusterRayDataPlugin_deserialize_key(
        PRESTypePluginEndpointData endpoint_data,
        OusterRayData ** sample,
        RTIBool * drop_sample,
        struct RTICdrStream *cdrStream,
        RTIBool deserialize_encapsulation,
        RTIBool deserialize_key,
        void *endpoint_plugin_qos);

    #define OusterSingleFiringPlugin_get_sample PRESTypePluginDefaultEndpointData_getSample

    #define OusterSingleFiringPlugin_get_buffer PRESTypePluginDefaultEndpointData_getBuffer 
    #define OusterSingleFiringPlugin_return_buffer PRESTypePluginDefaultEndpointData_returnBuffer

    #define OusterSingleFiringPlugin_create_sample PRESTypePluginDefaultEndpointData_createSample 
    #define OusterSingleFiringPlugin_destroy_sample PRESTypePluginDefaultEndpointData_deleteSample 

    /* --------------------------------------------------------------------------------------
    Support functions:
    * -------------------------------------------------------------------------------------- */

    NDDSUSERDllExport extern OusterSingleFiring*
    OusterSingleFiringPluginSupport_create_data_w_params(
        const struct DDS_TypeAllocationParams_t * alloc_params);

    NDDSUSERDllExport extern OusterSingleFiring*
    OusterSingleFiringPluginSupport_create_data_ex(RTIBool allocate_pointers);

    NDDSUSERDllExport extern OusterSingleFiring*
    OusterSingleFiringPluginSupport_create_data(void);

    NDDSUSERDllExport extern RTIBool 
    OusterSingleFiringPluginSupport_copy_data(
        OusterSingleFiring *out,
        const OusterSingleFiring *in);

    NDDSUSERDllExport extern void 
    OusterSingleFiringPluginSupport_destroy_data_w_params(
        OusterSingleFiring *sample,
        const struct DDS_TypeDeallocationParams_t * dealloc_params);

    NDDSUSERDllExport extern void 
    OusterSingleFiringPluginSupport_destroy_data_ex(
        OusterSingleFiring *sample,RTIBool deallocate_pointers);

    NDDSUSERDllExport extern void 
    OusterSingleFiringPluginSupport_destroy_data(
        OusterSingleFiring *sample);

    NDDSUSERDllExport extern void 
    OusterSingleFiringPluginSupport_print_data(
        const OusterSingleFiring *sample,
        const char *desc,
        unsigned int indent);

    /* ----------------------------------------------------------------------------
    Callback functions:
    * ---------------------------------------------------------------------------- */

    NDDSUSERDllExport extern RTIBool 
    OusterSingleFiringPlugin_copy_sample(
        PRESTypePluginEndpointData endpoint_data,
        OusterSingleFiring *out,
        const OusterSingleFiring *in);

    /* ----------------------------------------------------------------------------
    (De)Serialize functions:
    * ------------------------------------------------------------------------- */

    NDDSUSERDllExport extern RTIBool
    OusterSingleFiringPlugin_serialize_to_cdr_buffer(
        char * buffer,
        unsigned int * length,
        const OusterSingleFiring *sample,
        ::dds::core::policy::DataRepresentationId representation
        = ::dds::core::policy::DataRepresentation::xcdr()); 

    NDDSUSERDllExport extern RTIBool 
    OusterSingleFiringPlugin_deserialize(
        PRESTypePluginEndpointData endpoint_data,
        OusterSingleFiring **sample, 
        RTIBool * drop_sample,
        struct RTICdrStream *cdrStream,
        RTIBool deserialize_encapsulation,
        RTIBool deserialize_sample, 
        void *endpoint_plugin_qos);

    NDDSUSERDllExport extern RTIBool
    OusterSingleFiringPlugin_deserialize_from_cdr_buffer(
        OusterSingleFiring *sample,
        const char * buffer,
        unsigned int length);    

    NDDSUSERDllExport extern unsigned int 
    OusterSingleFiringPlugin_get_serialized_sample_max_size(
        PRESTypePluginEndpointData endpoint_data,
        RTIBool include_encapsulation,
        RTIEncapsulationId encapsulation_id,
        unsigned int current_alignment);

    /* --------------------------------------------------------------------------------------
    Key Management functions:
    * -------------------------------------------------------------------------------------- */
    NDDSUSERDllExport extern PRESTypePluginKeyKind 
    OusterSingleFiringPlugin_get_key_kind(void);

    NDDSUSERDllExport extern unsigned int 
    OusterSingleFiringPlugin_get_serialized_key_max_size(
        PRESTypePluginEndpointData endpoint_data,
        RTIBool include_encapsulation,
        RTIEncapsulationId encapsulation_id,
        unsigned int current_alignment);

    NDDSUSERDllExport extern unsigned int 
    OusterSingleFiringPlugin_get_serialized_key_max_size_for_keyhash(
        PRESTypePluginEndpointData endpoint_data,
        RTIEncapsulationId encapsulation_id,
        unsigned int current_alignment);

    NDDSUSERDllExport extern RTIBool 
    OusterSingleFiringPlugin_deserialize_key(
        PRESTypePluginEndpointData endpoint_data,
        OusterSingleFiring ** sample,
        RTIBool * drop_sample,
        struct RTICdrStream *cdrStream,
        RTIBool deserialize_encapsulation,
        RTIBool deserialize_key,
        void *endpoint_plugin_qos);

    #define OusterMsgPlugin_get_sample PRESTypePluginDefaultEndpointData_getSample

    #define OusterMsgPlugin_get_buffer PRESTypePluginDefaultEndpointData_getBuffer 
    #define OusterMsgPlugin_return_buffer PRESTypePluginDefaultEndpointData_returnBuffer

    #define OusterMsgPlugin_create_sample PRESTypePluginDefaultEndpointData_createSample 
    #define OusterMsgPlugin_destroy_sample PRESTypePluginDefaultEndpointData_deleteSample 

    /* --------------------------------------------------------------------------------------
    Support functions:
    * -------------------------------------------------------------------------------------- */

    NDDSUSERDllExport extern OusterMsg*
    OusterMsgPluginSupport_create_data_w_params(
        const struct DDS_TypeAllocationParams_t * alloc_params);

    NDDSUSERDllExport extern OusterMsg*
    OusterMsgPluginSupport_create_data_ex(RTIBool allocate_pointers);

    NDDSUSERDllExport extern OusterMsg*
    OusterMsgPluginSupport_create_data(void);

    NDDSUSERDllExport extern RTIBool 
    OusterMsgPluginSupport_copy_data(
        OusterMsg *out,
        const OusterMsg *in);

    NDDSUSERDllExport extern void 
    OusterMsgPluginSupport_destroy_data_w_params(
        OusterMsg *sample,
        const struct DDS_TypeDeallocationParams_t * dealloc_params);

    NDDSUSERDllExport extern void 
    OusterMsgPluginSupport_destroy_data_ex(
        OusterMsg *sample,RTIBool deallocate_pointers);

    NDDSUSERDllExport extern void 
    OusterMsgPluginSupport_destroy_data(
        OusterMsg *sample);

    NDDSUSERDllExport extern void 
    OusterMsgPluginSupport_print_data(
        const OusterMsg *sample,
        const char *desc,
        unsigned int indent);

    /* ----------------------------------------------------------------------------
    Callback functions:
    * ---------------------------------------------------------------------------- */

    NDDSUSERDllExport extern PRESTypePluginParticipantData 
    OusterMsgPlugin_on_participant_attached(
        void *registration_data, 
        const struct PRESTypePluginParticipantInfo *participant_info,
        RTIBool top_level_registration, 
        void *container_plugin_context,
        RTICdrTypeCode *typeCode);

    NDDSUSERDllExport extern void 
    OusterMsgPlugin_on_participant_detached(
        PRESTypePluginParticipantData participant_data);

    NDDSUSERDllExport extern PRESTypePluginEndpointData 
    OusterMsgPlugin_on_endpoint_attached(
        PRESTypePluginParticipantData participant_data,
        const struct PRESTypePluginEndpointInfo *endpoint_info,
        RTIBool top_level_registration, 
        void *container_plugin_context);

    NDDSUSERDllExport extern void 
    OusterMsgPlugin_on_endpoint_detached(
        PRESTypePluginEndpointData endpoint_data);

    NDDSUSERDllExport extern void    
    OusterMsgPlugin_return_sample(
        PRESTypePluginEndpointData endpoint_data,
        OusterMsg *sample,
        void *handle);    

    NDDSUSERDllExport extern RTIBool 
    OusterMsgPlugin_copy_sample(
        PRESTypePluginEndpointData endpoint_data,
        OusterMsg *out,
        const OusterMsg *in);

    /* ----------------------------------------------------------------------------
    (De)Serialize functions:
    * ------------------------------------------------------------------------- */

    NDDSUSERDllExport extern RTIBool
    OusterMsgPlugin_serialize_to_cdr_buffer(
        char * buffer,
        unsigned int * length,
        const OusterMsg *sample,
        ::dds::core::policy::DataRepresentationId representation
        = ::dds::core::policy::DataRepresentation::xcdr()); 

    NDDSUSERDllExport extern RTIBool 
    OusterMsgPlugin_deserialize(
        PRESTypePluginEndpointData endpoint_data,
        OusterMsg **sample, 
        RTIBool * drop_sample,
        struct RTICdrStream *cdrStream,
        RTIBool deserialize_encapsulation,
        RTIBool deserialize_sample, 
        void *endpoint_plugin_qos);

    NDDSUSERDllExport extern RTIBool
    OusterMsgPlugin_deserialize_from_cdr_buffer(
        OusterMsg *sample,
        const char * buffer,
        unsigned int length);    

    NDDSUSERDllExport extern unsigned int 
    OusterMsgPlugin_get_serialized_sample_max_size(
        PRESTypePluginEndpointData endpoint_data,
        RTIBool include_encapsulation,
        RTIEncapsulationId encapsulation_id,
        unsigned int current_alignment);

    /* --------------------------------------------------------------------------------------
    Key Management functions:
    * -------------------------------------------------------------------------------------- */
    NDDSUSERDllExport extern PRESTypePluginKeyKind 
    OusterMsgPlugin_get_key_kind(void);

    NDDSUSERDllExport extern unsigned int 
    OusterMsgPlugin_get_serialized_key_max_size(
        PRESTypePluginEndpointData endpoint_data,
        RTIBool include_encapsulation,
        RTIEncapsulationId encapsulation_id,
        unsigned int current_alignment);

    NDDSUSERDllExport extern unsigned int 
    OusterMsgPlugin_get_serialized_key_max_size_for_keyhash(
        PRESTypePluginEndpointData endpoint_data,
        RTIEncapsulationId encapsulation_id,
        unsigned int current_alignment);

    NDDSUSERDllExport extern RTIBool 
    OusterMsgPlugin_deserialize_key(
        PRESTypePluginEndpointData endpoint_data,
        OusterMsg ** sample,
        RTIBool * drop_sample,
        struct RTICdrStream *cdrStream,
        RTIBool deserialize_encapsulation,
        RTIBool deserialize_key,
        void *endpoint_plugin_qos);

    /* Plugin Functions */
    NDDSUSERDllExport extern struct PRESTypePlugin*
    OusterMsgPlugin_new(void);

    NDDSUSERDllExport extern void
    OusterMsgPlugin_delete(struct PRESTypePlugin *);

} /* namespace Ouster  */

#if (defined(RTI_WIN32) || defined (RTI_WINCE) || defined(RTI_INTIME)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, stop exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport
#endif

#endif /* ouster_dds_modelPlugin_1522706435_h */

