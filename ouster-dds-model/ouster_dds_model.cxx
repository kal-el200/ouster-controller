

/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from ouster_dds_model.idl
using RTI Code Generator (rtiddsgen) version 4.3.0.
The rtiddsgen tool is part of the RTI Connext DDS distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the Code Generator User's Manual.
*/

#include <iosfwd>
#include <iomanip>

#ifndef NDDS_STANDALONE_TYPE
#include "rti/topic/cdr/Serialization.hpp"
#include "ouster_dds_modelPlugin.hpp"
#else
#include "rti/topic/cdr/SerializationHelpers.hpp"
#endif

#include "ouster_dds_model.hpp"

#include <rti/util/ostream_operators.hpp>

namespace Ouster {

    // ---- OusterRayData: 

    OusterRayData::OusterRayData() :
        m_distance_first_ (0) ,
        m_reflectivity_first_ (0) ,
        m_nir_value_ (0)  {

    }   

    OusterRayData::OusterRayData (uint16_t distance_first_,uint8_t reflectivity_first_,uint16_t nir_value_):
        m_distance_first_(distance_first_), 
        m_reflectivity_first_(reflectivity_first_), 
        m_nir_value_(nir_value_) {
    }

    void OusterRayData::swap(OusterRayData& other_)  noexcept 
    {
        using std::swap;
        swap(m_distance_first_, other_.m_distance_first_);
        swap(m_reflectivity_first_, other_.m_reflectivity_first_);
        swap(m_nir_value_, other_.m_nir_value_);
    }  

    bool OusterRayData::operator == (const OusterRayData& other_) const {
        if (m_distance_first_ != other_.m_distance_first_) {
            return false;
        }
        if (m_reflectivity_first_ != other_.m_reflectivity_first_) {
            return false;
        }
        if (m_nir_value_ != other_.m_nir_value_) {
            return false;
        }
        return true;
    }

    bool OusterRayData::operator != (const OusterRayData& other_) const {
        return !this->operator ==(other_);
    }

    std::ostream& operator << (std::ostream& o,const OusterRayData& sample)
    {
        ::rti::util::StreamFlagSaver flag_saver (o);
        o <<"[";
        o << "distance_first: " << sample.distance_first ()<<", ";
        o << "reflectivity_first: " << (int) sample.reflectivity_first ()<<", ";
        o << "nir_value: " << sample.nir_value ();
        o <<"]";
        return o;
    }

    // ---- OusterSingleFiring: 

    OusterSingleFiring::OusterSingleFiring() :
        m_rotational_direction_azimuth_ (0) ,
        m_azimuth_firing_time_ (0ll)  {

    }   

    OusterSingleFiring::OusterSingleFiring (uint16_t rotational_direction_azimuth_,const ::rti::core::bounded_sequence< ::Ouster::OusterRayData, 128L >& ouster_ray_data_,const ::rti::core::bounded_sequence< uint8_t, 128L >& reflectivity_second_,const ::rti::core::bounded_sequence< uint16_t, 128L >& distance_second_,int64_t azimuth_firing_time_):
        m_rotational_direction_azimuth_(rotational_direction_azimuth_), 
        m_ouster_ray_data_(ouster_ray_data_), 
        m_reflectivity_second_(reflectivity_second_), 
        m_distance_second_(distance_second_), 
        m_azimuth_firing_time_(azimuth_firing_time_) {
    }

    void OusterSingleFiring::swap(OusterSingleFiring& other_)  noexcept 
    {
        using std::swap;
        swap(m_rotational_direction_azimuth_, other_.m_rotational_direction_azimuth_);
        swap(m_ouster_ray_data_, other_.m_ouster_ray_data_);
        swap(m_reflectivity_second_, other_.m_reflectivity_second_);
        swap(m_distance_second_, other_.m_distance_second_);
        swap(m_azimuth_firing_time_, other_.m_azimuth_firing_time_);
    }  

    bool OusterSingleFiring::operator == (const OusterSingleFiring& other_) const {
        if (m_rotational_direction_azimuth_ != other_.m_rotational_direction_azimuth_) {
            return false;
        }
        if (m_ouster_ray_data_ != other_.m_ouster_ray_data_) {
            return false;
        }
        if (m_reflectivity_second_ != other_.m_reflectivity_second_) {
            return false;
        }
        if (m_distance_second_ != other_.m_distance_second_) {
            return false;
        }
        if (m_azimuth_firing_time_ != other_.m_azimuth_firing_time_) {
            return false;
        }
        return true;
    }

    bool OusterSingleFiring::operator != (const OusterSingleFiring& other_) const {
        return !this->operator ==(other_);
    }

    std::ostream& operator << (std::ostream& o,const OusterSingleFiring& sample)
    {
        ::rti::util::StreamFlagSaver flag_saver (o);
        o <<"[";
        o << "rotational_direction_azimuth: " << sample.rotational_direction_azimuth ()<<", ";
        o << "ouster_ray_data: " << sample.ouster_ray_data ()<<", ";
        o << "reflectivity_second: " << sample.reflectivity_second ()<<", ";
        o << "distance_second: " << sample.distance_second ()<<", ";
        o << "azimuth_firing_time: " << sample.azimuth_firing_time ();
        o <<"]";
        return o;
    }

    // ---- OusterMsg: 

    OusterMsg::OusterMsg() :
        m_msg_index_ (0ull) ,
        m_sensor_model_type_ (0) ,
        m_return_mode_ (0) ,
        m_beam_altitude_angle_type_ (0)  {

    }   

    OusterMsg::OusterMsg (uint64_t msg_index_,int8_t sensor_model_type_,int8_t return_mode_,const ::rti::core::bounded_sequence< ::Ouster::OusterSingleFiring, 600L >& ouster_packet_,uint8_t beam_altitude_angle_type_):
        m_msg_index_(msg_index_), 
        m_sensor_model_type_(sensor_model_type_), 
        m_return_mode_(return_mode_), 
        m_ouster_packet_(ouster_packet_), 
        m_beam_altitude_angle_type_(beam_altitude_angle_type_) {
    }

    void OusterMsg::swap(OusterMsg& other_)  noexcept 
    {
        using std::swap;
        swap(m_msg_index_, other_.m_msg_index_);
        swap(m_sensor_model_type_, other_.m_sensor_model_type_);
        swap(m_return_mode_, other_.m_return_mode_);
        swap(m_ouster_packet_, other_.m_ouster_packet_);
        swap(m_beam_altitude_angle_type_, other_.m_beam_altitude_angle_type_);
    }  

    bool OusterMsg::operator == (const OusterMsg& other_) const {
        if (m_msg_index_ != other_.m_msg_index_) {
            return false;
        }
        if (m_sensor_model_type_ != other_.m_sensor_model_type_) {
            return false;
        }
        if (m_return_mode_ != other_.m_return_mode_) {
            return false;
        }
        if (m_ouster_packet_ != other_.m_ouster_packet_) {
            return false;
        }
        if (m_beam_altitude_angle_type_ != other_.m_beam_altitude_angle_type_) {
            return false;
        }
        return true;
    }

    bool OusterMsg::operator != (const OusterMsg& other_) const {
        return !this->operator ==(other_);
    }

    std::ostream& operator << (std::ostream& o,const OusterMsg& sample)
    {
        ::rti::util::StreamFlagSaver flag_saver (o);
        o <<"[";
        o << "msg_index: " << sample.msg_index ()<<", ";
        o << "sensor_model_type: " << (int) sample.sensor_model_type ()<<", ";
        o << "return_mode: " << (int) sample.return_mode ()<<", ";
        o << "ouster_packet: " << sample.ouster_packet ()<<", ";
        o << "beam_altitude_angle_type: " << (int) sample.beam_altitude_angle_type ();
        o <<"]";
        return o;
    }

} // namespace Ouster  

#ifdef NDDS_STANDALONE_TYPE
namespace rti {
    namespace topic {
    }
}

#else
// --- Type traits: -------------------------------------------------

namespace rti { 
    namespace topic {

        template<>
        struct native_type_code< ::Ouster::OusterRayData > {
            static DDS_TypeCode * get()
            {
                using namespace ::rti::topic::interpreter;

                static RTIBool is_initialized = RTI_FALSE;

                static DDS_TypeCode_Member OusterRayData_g_tc_members[3]=
                {

                    {
                        (char *)"distance_first",/* Member name */
                        {
                            0,/* Representation ID */
                            DDS_BOOLEAN_FALSE,/* Is a pointer? */
                            -1, /* Bitfield bits */
                            NULL/* Member type code is assigned later */
                        },
                        0, /* Ignored */
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER
                    }, 
                    {
                        (char *)"reflectivity_first",/* Member name */
                        {
                            1,/* Representation ID */
                            DDS_BOOLEAN_FALSE,/* Is a pointer? */
                            -1, /* Bitfield bits */
                            NULL/* Member type code is assigned later */
                        },
                        0, /* Ignored */
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER
                    }, 
                    {
                        (char *)"nir_value",/* Member name */
                        {
                            2,/* Representation ID */
                            DDS_BOOLEAN_FALSE,/* Is a pointer? */
                            -1, /* Bitfield bits */
                            NULL/* Member type code is assigned later */
                        },
                        0, /* Ignored */
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER
                    }
                };

                static DDS_TypeCode OusterRayData_g_tc =
                {{
                        DDS_TK_STRUCT, /* Kind */
                        DDS_BOOLEAN_FALSE, /* Ignored */
                        -1, /*Ignored*/
                        (char *)"Ouster::OusterRayData", /* Name */
                        NULL, /* Ignored */      
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        3, /* Number of members */
                        OusterRayData_g_tc_members, /* Members */
                        DDS_VM_NONE, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER,
                        DDS_BOOLEAN_TRUE, /* _isCopyable */
                        NULL, /* _sampleAccessInfo: assigned later */
                        NULL /* _typePlugin: assigned later */
                    }}; /* Type code for OusterRayData*/

                if (is_initialized) {
                    return &OusterRayData_g_tc;
                }

                is_initialized = RTI_TRUE;

                OusterRayData_g_tc._data._annotations._allowedDataRepresentationMask = 5;

                OusterRayData_g_tc_members[0]._representation._typeCode = (RTICdrTypeCode *)&DDS_g_tc_ushort;
                OusterRayData_g_tc_members[1]._representation._typeCode = (RTICdrTypeCode *)&DDS_g_tc_octet;
                OusterRayData_g_tc_members[2]._representation._typeCode = (RTICdrTypeCode *)&DDS_g_tc_ushort;

                /* Initialize the values for member annotations. */
                OusterRayData_g_tc_members[0]._annotations._defaultValue._d = RTI_XCDR_TK_USHORT;
                OusterRayData_g_tc_members[0]._annotations._defaultValue._u.ushort_value = 0;
                OusterRayData_g_tc_members[0]._annotations._minValue._d = RTI_XCDR_TK_USHORT;
                OusterRayData_g_tc_members[0]._annotations._minValue._u.ushort_value = RTIXCdrUnsignedShort_MIN;
                OusterRayData_g_tc_members[0]._annotations._maxValue._d = RTI_XCDR_TK_USHORT;
                OusterRayData_g_tc_members[0]._annotations._maxValue._u.ushort_value = RTIXCdrUnsignedShort_MAX;
                OusterRayData_g_tc_members[1]._annotations._defaultValue._d = RTI_XCDR_TK_OCTET;
                OusterRayData_g_tc_members[1]._annotations._defaultValue._u.octet_value = 0;
                OusterRayData_g_tc_members[1]._annotations._minValue._d = RTI_XCDR_TK_OCTET;
                OusterRayData_g_tc_members[1]._annotations._minValue._u.octet_value = RTIXCdrOctet_MIN;
                OusterRayData_g_tc_members[1]._annotations._maxValue._d = RTI_XCDR_TK_OCTET;
                OusterRayData_g_tc_members[1]._annotations._maxValue._u.octet_value = RTIXCdrOctet_MAX;
                OusterRayData_g_tc_members[2]._annotations._defaultValue._d = RTI_XCDR_TK_USHORT;
                OusterRayData_g_tc_members[2]._annotations._defaultValue._u.ushort_value = 0;
                OusterRayData_g_tc_members[2]._annotations._minValue._d = RTI_XCDR_TK_USHORT;
                OusterRayData_g_tc_members[2]._annotations._minValue._u.ushort_value = RTIXCdrUnsignedShort_MIN;
                OusterRayData_g_tc_members[2]._annotations._maxValue._d = RTI_XCDR_TK_USHORT;
                OusterRayData_g_tc_members[2]._annotations._maxValue._u.ushort_value = RTIXCdrUnsignedShort_MAX;

                OusterRayData_g_tc._data._sampleAccessInfo = sample_access_info();
                OusterRayData_g_tc._data._typePlugin = type_plugin_info();    

                return &OusterRayData_g_tc;
            }

            static RTIXCdrSampleAccessInfo * sample_access_info()
            {
                static RTIBool is_initialized = RTI_FALSE;

                ::Ouster::OusterRayData *sample;

                static RTIXCdrMemberAccessInfo OusterRayData_g_memberAccessInfos[3] =
                {RTIXCdrMemberAccessInfo_INITIALIZER};

                static RTIXCdrSampleAccessInfo OusterRayData_g_sampleAccessInfo = 
                RTIXCdrSampleAccessInfo_INITIALIZER;

                if (is_initialized) {
                    return (RTIXCdrSampleAccessInfo*) &OusterRayData_g_sampleAccessInfo;
                }

                RTIXCdrHeap_allocateStruct(
                    &sample, 
                    ::Ouster::OusterRayData);
                if (sample == NULL) {
                    return NULL;
                }

                OusterRayData_g_memberAccessInfos[0].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->distance_first() - (char *)sample);

                OusterRayData_g_memberAccessInfos[1].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->reflectivity_first() - (char *)sample);

                OusterRayData_g_memberAccessInfos[2].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->nir_value() - (char *)sample);

                OusterRayData_g_sampleAccessInfo.memberAccessInfos = 
                OusterRayData_g_memberAccessInfos;

                {
                    size_t candidateTypeSize = sizeof(::Ouster::OusterRayData);

                    if (candidateTypeSize > RTIXCdrLong_MAX) {
                        OusterRayData_g_sampleAccessInfo.typeSize[0] =
                        RTIXCdrLong_MAX;
                    } else {
                        OusterRayData_g_sampleAccessInfo.typeSize[0] =
                        (RTIXCdrUnsignedLong) candidateTypeSize;
                    }
                }

                OusterRayData_g_sampleAccessInfo.useGetMemberValueOnlyWithRef =
                RTI_XCDR_TRUE;

                OusterRayData_g_sampleAccessInfo.getMemberValuePointerFcn = 
                interpreter::get_aggregation_value_pointer< ::Ouster::OusterRayData >;

                OusterRayData_g_sampleAccessInfo.languageBinding = 
                RTI_XCDR_TYPE_BINDING_CPP_11_STL ;

                RTIXCdrHeap_freeStruct(sample);
                is_initialized = RTI_TRUE;
                return (RTIXCdrSampleAccessInfo*) &OusterRayData_g_sampleAccessInfo;
            }
            static RTIXCdrTypePlugin * type_plugin_info()
            {
                static RTIXCdrTypePlugin OusterRayData_g_typePlugin = 
                {
                    NULL, /* serialize */
                    NULL, /* serialize_key */
                    NULL, /* deserialize_sample */
                    NULL, /* deserialize_key_sample */
                    NULL, /* skip */
                    NULL, /* get_serialized_sample_size */
                    NULL, /* get_serialized_sample_max_size_ex */
                    NULL, /* get_serialized_key_max_size_ex */
                    NULL, /* get_serialized_sample_min_size */
                    NULL, /* serialized_sample_to_key */
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL
                };

                return &OusterRayData_g_typePlugin;
            }
        }; // native_type_code

        const ::dds::core::xtypes::StructType& dynamic_type< ::Ouster::OusterRayData >::get()
        {
            return static_cast<const ::dds::core::xtypes::StructType&>(
                ::rti::core::native_conversions::cast_from_native< ::dds::core::xtypes::DynamicType >(
                    *(native_type_code< ::Ouster::OusterRayData >::get())));
        }

        template<>
        struct native_type_code< ::Ouster::OusterSingleFiring > {
            static DDS_TypeCode * get()
            {
                using namespace ::rti::topic::interpreter;

                static RTIBool is_initialized = RTI_FALSE;

                static DDS_TypeCode OusterSingleFiring_g_tc_ouster_ray_data_sequence;
                static DDS_TypeCode OusterSingleFiring_g_tc_reflectivity_second_sequence;
                static DDS_TypeCode OusterSingleFiring_g_tc_distance_second_sequence;

                static DDS_TypeCode_Member OusterSingleFiring_g_tc_members[5]=
                {

                    {
                        (char *)"rotational_direction_azimuth",/* Member name */
                        {
                            0,/* Representation ID */
                            DDS_BOOLEAN_FALSE,/* Is a pointer? */
                            -1, /* Bitfield bits */
                            NULL/* Member type code is assigned later */
                        },
                        0, /* Ignored */
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER
                    }, 
                    {
                        (char *)"ouster_ray_data",/* Member name */
                        {
                            1,/* Representation ID */
                            DDS_BOOLEAN_FALSE,/* Is a pointer? */
                            -1, /* Bitfield bits */
                            NULL/* Member type code is assigned later */
                        },
                        0, /* Ignored */
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER
                    }, 
                    {
                        (char *)"reflectivity_second",/* Member name */
                        {
                            2,/* Representation ID */
                            DDS_BOOLEAN_FALSE,/* Is a pointer? */
                            -1, /* Bitfield bits */
                            NULL/* Member type code is assigned later */
                        },
                        0, /* Ignored */
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER
                    }, 
                    {
                        (char *)"distance_second",/* Member name */
                        {
                            3,/* Representation ID */
                            DDS_BOOLEAN_FALSE,/* Is a pointer? */
                            -1, /* Bitfield bits */
                            NULL/* Member type code is assigned later */
                        },
                        0, /* Ignored */
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER
                    }, 
                    {
                        (char *)"azimuth_firing_time",/* Member name */
                        {
                            4,/* Representation ID */
                            DDS_BOOLEAN_FALSE,/* Is a pointer? */
                            -1, /* Bitfield bits */
                            NULL/* Member type code is assigned later */
                        },
                        0, /* Ignored */
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER
                    }
                };

                static DDS_TypeCode OusterSingleFiring_g_tc =
                {{
                        DDS_TK_STRUCT, /* Kind */
                        DDS_BOOLEAN_FALSE, /* Ignored */
                        -1, /*Ignored*/
                        (char *)"Ouster::OusterSingleFiring", /* Name */
                        NULL, /* Ignored */      
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        5, /* Number of members */
                        OusterSingleFiring_g_tc_members, /* Members */
                        DDS_VM_NONE, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER,
                        DDS_BOOLEAN_TRUE, /* _isCopyable */
                        NULL, /* _sampleAccessInfo: assigned later */
                        NULL /* _typePlugin: assigned later */
                    }}; /* Type code for OusterSingleFiring*/

                if (is_initialized) {
                    return &OusterSingleFiring_g_tc;
                }

                is_initialized = RTI_TRUE;

                OusterSingleFiring_g_tc_ouster_ray_data_sequence = initialize_sequence_typecode< ::rti::core::bounded_sequence< ::Ouster::OusterRayData, 128L > >((128L));
                OusterSingleFiring_g_tc_reflectivity_second_sequence = initialize_sequence_typecode< ::rti::core::bounded_sequence< uint8_t, 128L > >((128L));
                OusterSingleFiring_g_tc_distance_second_sequence = initialize_sequence_typecode< ::rti::core::bounded_sequence< uint16_t, 128L > >((128L));

                OusterSingleFiring_g_tc._data._annotations._allowedDataRepresentationMask = 5;

                OusterSingleFiring_g_tc_ouster_ray_data_sequence._data._typeCode = (RTICdrTypeCode *)&::rti::topic::dynamic_type< ::Ouster::OusterRayData>::get().native();
                OusterSingleFiring_g_tc_reflectivity_second_sequence._data._typeCode = (RTICdrTypeCode *)&DDS_g_tc_octet;
                OusterSingleFiring_g_tc_distance_second_sequence._data._typeCode = (RTICdrTypeCode *)&DDS_g_tc_ushort;
                OusterSingleFiring_g_tc_members[0]._representation._typeCode = (RTICdrTypeCode *)&DDS_g_tc_ushort;
                OusterSingleFiring_g_tc_members[1]._representation._typeCode = (RTICdrTypeCode *)& OusterSingleFiring_g_tc_ouster_ray_data_sequence;
                OusterSingleFiring_g_tc_members[2]._representation._typeCode = (RTICdrTypeCode *)& OusterSingleFiring_g_tc_reflectivity_second_sequence;
                OusterSingleFiring_g_tc_members[3]._representation._typeCode = (RTICdrTypeCode *)& OusterSingleFiring_g_tc_distance_second_sequence;
                OusterSingleFiring_g_tc_members[4]._representation._typeCode = (RTICdrTypeCode *)&DDS_g_tc_longlong;

                /* Initialize the values for member annotations. */
                OusterSingleFiring_g_tc_members[0]._annotations._defaultValue._d = RTI_XCDR_TK_USHORT;
                OusterSingleFiring_g_tc_members[0]._annotations._defaultValue._u.ushort_value = 0;
                OusterSingleFiring_g_tc_members[0]._annotations._minValue._d = RTI_XCDR_TK_USHORT;
                OusterSingleFiring_g_tc_members[0]._annotations._minValue._u.ushort_value = RTIXCdrUnsignedShort_MIN;
                OusterSingleFiring_g_tc_members[0]._annotations._maxValue._d = RTI_XCDR_TK_USHORT;
                OusterSingleFiring_g_tc_members[0]._annotations._maxValue._u.ushort_value = RTIXCdrUnsignedShort_MAX;
                OusterSingleFiring_g_tc_members[4]._annotations._defaultValue._d = RTI_XCDR_TK_LONGLONG;
                OusterSingleFiring_g_tc_members[4]._annotations._defaultValue._u.long_long_value = 0ll;
                OusterSingleFiring_g_tc_members[4]._annotations._minValue._d = RTI_XCDR_TK_LONGLONG;
                OusterSingleFiring_g_tc_members[4]._annotations._minValue._u.long_long_value = RTIXCdrLongLong_MIN;
                OusterSingleFiring_g_tc_members[4]._annotations._maxValue._d = RTI_XCDR_TK_LONGLONG;
                OusterSingleFiring_g_tc_members[4]._annotations._maxValue._u.long_long_value = RTIXCdrLongLong_MAX;

                OusterSingleFiring_g_tc._data._sampleAccessInfo = sample_access_info();
                OusterSingleFiring_g_tc._data._typePlugin = type_plugin_info();    

                return &OusterSingleFiring_g_tc;
            }

            static RTIXCdrSampleAccessInfo * sample_access_info()
            {
                static RTIBool is_initialized = RTI_FALSE;

                ::Ouster::OusterSingleFiring *sample;

                static RTIXCdrMemberAccessInfo OusterSingleFiring_g_memberAccessInfos[5] =
                {RTIXCdrMemberAccessInfo_INITIALIZER};

                static RTIXCdrSampleAccessInfo OusterSingleFiring_g_sampleAccessInfo = 
                RTIXCdrSampleAccessInfo_INITIALIZER;

                if (is_initialized) {
                    return (RTIXCdrSampleAccessInfo*) &OusterSingleFiring_g_sampleAccessInfo;
                }

                RTIXCdrHeap_allocateStruct(
                    &sample, 
                    ::Ouster::OusterSingleFiring);
                if (sample == NULL) {
                    return NULL;
                }

                OusterSingleFiring_g_memberAccessInfos[0].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->rotational_direction_azimuth() - (char *)sample);

                OusterSingleFiring_g_memberAccessInfos[1].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->ouster_ray_data() - (char *)sample);

                OusterSingleFiring_g_memberAccessInfos[2].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->reflectivity_second() - (char *)sample);

                OusterSingleFiring_g_memberAccessInfos[3].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->distance_second() - (char *)sample);

                OusterSingleFiring_g_memberAccessInfos[4].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->azimuth_firing_time() - (char *)sample);

                OusterSingleFiring_g_sampleAccessInfo.memberAccessInfos = 
                OusterSingleFiring_g_memberAccessInfos;

                {
                    size_t candidateTypeSize = sizeof(::Ouster::OusterSingleFiring);

                    if (candidateTypeSize > RTIXCdrLong_MAX) {
                        OusterSingleFiring_g_sampleAccessInfo.typeSize[0] =
                        RTIXCdrLong_MAX;
                    } else {
                        OusterSingleFiring_g_sampleAccessInfo.typeSize[0] =
                        (RTIXCdrUnsignedLong) candidateTypeSize;
                    }
                }

                OusterSingleFiring_g_sampleAccessInfo.useGetMemberValueOnlyWithRef =
                RTI_XCDR_TRUE;

                OusterSingleFiring_g_sampleAccessInfo.getMemberValuePointerFcn = 
                interpreter::get_aggregation_value_pointer< ::Ouster::OusterSingleFiring >;

                OusterSingleFiring_g_sampleAccessInfo.languageBinding = 
                RTI_XCDR_TYPE_BINDING_CPP_11_STL ;

                RTIXCdrHeap_freeStruct(sample);
                is_initialized = RTI_TRUE;
                return (RTIXCdrSampleAccessInfo*) &OusterSingleFiring_g_sampleAccessInfo;
            }
            static RTIXCdrTypePlugin * type_plugin_info()
            {
                static RTIXCdrTypePlugin OusterSingleFiring_g_typePlugin = 
                {
                    NULL, /* serialize */
                    NULL, /* serialize_key */
                    NULL, /* deserialize_sample */
                    NULL, /* deserialize_key_sample */
                    NULL, /* skip */
                    NULL, /* get_serialized_sample_size */
                    NULL, /* get_serialized_sample_max_size_ex */
                    NULL, /* get_serialized_key_max_size_ex */
                    NULL, /* get_serialized_sample_min_size */
                    NULL, /* serialized_sample_to_key */
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL
                };

                return &OusterSingleFiring_g_typePlugin;
            }
        }; // native_type_code

        const ::dds::core::xtypes::StructType& dynamic_type< ::Ouster::OusterSingleFiring >::get()
        {
            return static_cast<const ::dds::core::xtypes::StructType&>(
                ::rti::core::native_conversions::cast_from_native< ::dds::core::xtypes::DynamicType >(
                    *(native_type_code< ::Ouster::OusterSingleFiring >::get())));
        }

        template<>
        struct native_type_code< ::Ouster::OusterMsg > {
            static DDS_TypeCode * get()
            {
                using namespace ::rti::topic::interpreter;

                static RTIBool is_initialized = RTI_FALSE;

                static DDS_TypeCode OusterMsg_g_tc_ouster_packet_sequence;

                static DDS_TypeCode_Member OusterMsg_g_tc_members[5]=
                {

                    {
                        (char *)"msg_index",/* Member name */
                        {
                            0,/* Representation ID */
                            DDS_BOOLEAN_FALSE,/* Is a pointer? */
                            -1, /* Bitfield bits */
                            NULL/* Member type code is assigned later */
                        },
                        0, /* Ignored */
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER
                    }, 
                    {
                        (char *)"sensor_model_type",/* Member name */
                        {
                            1,/* Representation ID */
                            DDS_BOOLEAN_FALSE,/* Is a pointer? */
                            -1, /* Bitfield bits */
                            NULL/* Member type code is assigned later */
                        },
                        0, /* Ignored */
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER
                    }, 
                    {
                        (char *)"return_mode",/* Member name */
                        {
                            2,/* Representation ID */
                            DDS_BOOLEAN_FALSE,/* Is a pointer? */
                            -1, /* Bitfield bits */
                            NULL/* Member type code is assigned later */
                        },
                        0, /* Ignored */
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER
                    }, 
                    {
                        (char *)"ouster_packet",/* Member name */
                        {
                            3,/* Representation ID */
                            DDS_BOOLEAN_FALSE,/* Is a pointer? */
                            -1, /* Bitfield bits */
                            NULL/* Member type code is assigned later */
                        },
                        0, /* Ignored */
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER
                    }, 
                    {
                        (char *)"beam_altitude_angle_type",/* Member name */
                        {
                            4,/* Representation ID */
                            DDS_BOOLEAN_FALSE,/* Is a pointer? */
                            -1, /* Bitfield bits */
                            NULL/* Member type code is assigned later */
                        },
                        0, /* Ignored */
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
                        DDS_PUBLIC_MEMBER,/* Member visibility */
                        1,
                        NULL, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER
                    }
                };

                static DDS_TypeCode OusterMsg_g_tc =
                {{
                        DDS_TK_STRUCT, /* Kind */
                        DDS_BOOLEAN_FALSE, /* Ignored */
                        -1, /*Ignored*/
                        (char *)"Ouster::OusterMsg", /* Name */
                        NULL, /* Ignored */      
                        0, /* Ignored */
                        0, /* Ignored */
                        NULL, /* Ignored */
                        5, /* Number of members */
                        OusterMsg_g_tc_members, /* Members */
                        DDS_VM_NONE, /* Ignored */
                        RTICdrTypeCodeAnnotations_INITIALIZER,
                        DDS_BOOLEAN_TRUE, /* _isCopyable */
                        NULL, /* _sampleAccessInfo: assigned later */
                        NULL /* _typePlugin: assigned later */
                    }}; /* Type code for OusterMsg*/

                if (is_initialized) {
                    return &OusterMsg_g_tc;
                }

                is_initialized = RTI_TRUE;

                OusterMsg_g_tc_ouster_packet_sequence = initialize_sequence_typecode< ::rti::core::bounded_sequence< ::Ouster::OusterSingleFiring, 600L > >((600L));

                OusterMsg_g_tc._data._annotations._allowedDataRepresentationMask = 5;

                OusterMsg_g_tc_ouster_packet_sequence._data._typeCode = (RTICdrTypeCode *)&::rti::topic::dynamic_type< ::Ouster::OusterSingleFiring>::get().native();
                OusterMsg_g_tc_members[0]._representation._typeCode = (RTICdrTypeCode *)&DDS_g_tc_ulonglong;
                OusterMsg_g_tc_members[1]._representation._typeCode = (RTICdrTypeCode *)&DDS_g_tc_octet;
                OusterMsg_g_tc_members[2]._representation._typeCode = (RTICdrTypeCode *)&DDS_g_tc_octet;
                OusterMsg_g_tc_members[3]._representation._typeCode = (RTICdrTypeCode *)& OusterMsg_g_tc_ouster_packet_sequence;
                OusterMsg_g_tc_members[4]._representation._typeCode = (RTICdrTypeCode *)&DDS_g_tc_octet;

                /* Initialize the values for member annotations. */
                OusterMsg_g_tc_members[0]._annotations._defaultValue._d = RTI_XCDR_TK_ULONGLONG;
                OusterMsg_g_tc_members[0]._annotations._defaultValue._u.ulong_long_value = 0ull;
                OusterMsg_g_tc_members[0]._annotations._minValue._d = RTI_XCDR_TK_ULONGLONG;
                OusterMsg_g_tc_members[0]._annotations._minValue._u.ulong_long_value = RTIXCdrUnsignedLongLong_MIN;
                OusterMsg_g_tc_members[0]._annotations._maxValue._d = RTI_XCDR_TK_ULONGLONG;
                OusterMsg_g_tc_members[0]._annotations._maxValue._u.ulong_long_value = RTIXCdrUnsignedLongLong_MAX;
                OusterMsg_g_tc_members[1]._annotations._defaultValue._d = RTI_XCDR_TK_OCTET;
                OusterMsg_g_tc_members[1]._annotations._defaultValue._u.octet_value = 0;
                OusterMsg_g_tc_members[1]._annotations._minValue._d = RTI_XCDR_TK_OCTET;
                OusterMsg_g_tc_members[1]._annotations._minValue._u.octet_value = RTIXCdrOctet_MIN;
                OusterMsg_g_tc_members[1]._annotations._maxValue._d = RTI_XCDR_TK_OCTET;
                OusterMsg_g_tc_members[1]._annotations._maxValue._u.octet_value = RTIXCdrOctet_MAX;
                OusterMsg_g_tc_members[2]._annotations._defaultValue._d = RTI_XCDR_TK_OCTET;
                OusterMsg_g_tc_members[2]._annotations._defaultValue._u.octet_value = 0;
                OusterMsg_g_tc_members[2]._annotations._minValue._d = RTI_XCDR_TK_OCTET;
                OusterMsg_g_tc_members[2]._annotations._minValue._u.octet_value = RTIXCdrOctet_MIN;
                OusterMsg_g_tc_members[2]._annotations._maxValue._d = RTI_XCDR_TK_OCTET;
                OusterMsg_g_tc_members[2]._annotations._maxValue._u.octet_value = RTIXCdrOctet_MAX;
                OusterMsg_g_tc_members[4]._annotations._defaultValue._d = RTI_XCDR_TK_OCTET;
                OusterMsg_g_tc_members[4]._annotations._defaultValue._u.octet_value = 0;
                OusterMsg_g_tc_members[4]._annotations._minValue._d = RTI_XCDR_TK_OCTET;
                OusterMsg_g_tc_members[4]._annotations._minValue._u.octet_value = RTIXCdrOctet_MIN;
                OusterMsg_g_tc_members[4]._annotations._maxValue._d = RTI_XCDR_TK_OCTET;
                OusterMsg_g_tc_members[4]._annotations._maxValue._u.octet_value = RTIXCdrOctet_MAX;

                OusterMsg_g_tc._data._sampleAccessInfo = sample_access_info();
                OusterMsg_g_tc._data._typePlugin = type_plugin_info();    

                return &OusterMsg_g_tc;
            }

            static RTIXCdrSampleAccessInfo * sample_access_info()
            {
                static RTIBool is_initialized = RTI_FALSE;

                ::Ouster::OusterMsg *sample;

                static RTIXCdrMemberAccessInfo OusterMsg_g_memberAccessInfos[5] =
                {RTIXCdrMemberAccessInfo_INITIALIZER};

                static RTIXCdrSampleAccessInfo OusterMsg_g_sampleAccessInfo = 
                RTIXCdrSampleAccessInfo_INITIALIZER;

                if (is_initialized) {
                    return (RTIXCdrSampleAccessInfo*) &OusterMsg_g_sampleAccessInfo;
                }

                RTIXCdrHeap_allocateStruct(
                    &sample, 
                    ::Ouster::OusterMsg);
                if (sample == NULL) {
                    return NULL;
                }

                OusterMsg_g_memberAccessInfos[0].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->msg_index() - (char *)sample);

                OusterMsg_g_memberAccessInfos[1].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->sensor_model_type() - (char *)sample);

                OusterMsg_g_memberAccessInfos[2].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->return_mode() - (char *)sample);

                OusterMsg_g_memberAccessInfos[3].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->ouster_packet() - (char *)sample);

                OusterMsg_g_memberAccessInfos[4].bindingMemberValueOffset[0] = 
                (RTIXCdrUnsignedLong) ((char *)&sample->beam_altitude_angle_type() - (char *)sample);

                OusterMsg_g_sampleAccessInfo.memberAccessInfos = 
                OusterMsg_g_memberAccessInfos;

                {
                    size_t candidateTypeSize = sizeof(::Ouster::OusterMsg);

                    if (candidateTypeSize > RTIXCdrLong_MAX) {
                        OusterMsg_g_sampleAccessInfo.typeSize[0] =
                        RTIXCdrLong_MAX;
                    } else {
                        OusterMsg_g_sampleAccessInfo.typeSize[0] =
                        (RTIXCdrUnsignedLong) candidateTypeSize;
                    }
                }

                OusterMsg_g_sampleAccessInfo.useGetMemberValueOnlyWithRef =
                RTI_XCDR_TRUE;

                OusterMsg_g_sampleAccessInfo.getMemberValuePointerFcn = 
                interpreter::get_aggregation_value_pointer< ::Ouster::OusterMsg >;

                OusterMsg_g_sampleAccessInfo.languageBinding = 
                RTI_XCDR_TYPE_BINDING_CPP_11_STL ;

                RTIXCdrHeap_freeStruct(sample);
                is_initialized = RTI_TRUE;
                return (RTIXCdrSampleAccessInfo*) &OusterMsg_g_sampleAccessInfo;
            }
            static RTIXCdrTypePlugin * type_plugin_info()
            {
                static RTIXCdrTypePlugin OusterMsg_g_typePlugin = 
                {
                    NULL, /* serialize */
                    NULL, /* serialize_key */
                    NULL, /* deserialize_sample */
                    NULL, /* deserialize_key_sample */
                    NULL, /* skip */
                    NULL, /* get_serialized_sample_size */
                    NULL, /* get_serialized_sample_max_size_ex */
                    NULL, /* get_serialized_key_max_size_ex */
                    NULL, /* get_serialized_sample_min_size */
                    NULL, /* serialized_sample_to_key */
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL
                };

                return &OusterMsg_g_typePlugin;
            }
        }; // native_type_code

        const ::dds::core::xtypes::StructType& dynamic_type< ::Ouster::OusterMsg >::get()
        {
            return static_cast<const ::dds::core::xtypes::StructType&>(
                ::rti::core::native_conversions::cast_from_native< ::dds::core::xtypes::DynamicType >(
                    *(native_type_code< ::Ouster::OusterMsg >::get())));
        }
    }
}

namespace dds { 
    namespace topic {

        void topic_type_support< ::Ouster::OusterRayData >::reset_sample(::Ouster::OusterRayData& sample) 
        {
            sample.distance_first(0);
            sample.reflectivity_first(0);
            sample.nir_value(0);
        }

        void topic_type_support< ::Ouster::OusterRayData >::allocate_sample(::Ouster::OusterRayData& sample, int, int) 
        {
            RTIOsapiUtility_unusedParameter(sample);
        }

        void topic_type_support< ::Ouster::OusterSingleFiring >::reset_sample(::Ouster::OusterSingleFiring& sample) 
        {
            sample.rotational_direction_azimuth(0);
            ::rti::topic::reset_sample(sample.ouster_ray_data());
            ::rti::topic::reset_sample(sample.reflectivity_second());
            ::rti::topic::reset_sample(sample.distance_second());
            sample.azimuth_firing_time(0ll);
        }

        void topic_type_support< ::Ouster::OusterSingleFiring >::allocate_sample(::Ouster::OusterSingleFiring& sample, int, int) 
        {
            ::rti::topic::allocate_sample(sample.ouster_ray_data(),  128L, -1);
            ::rti::topic::allocate_sample(sample.reflectivity_second(),  128L, -1);
            ::rti::topic::allocate_sample(sample.distance_second(),  128L, -1);
        }
        void topic_type_support< ::Ouster::OusterMsg >:: register_type(
            ::dds::domain::DomainParticipant& participant,
            const std::string& type_name) 
        {

            ::rti::domain::register_type_plugin(
                participant,
                type_name,
                ::Ouster::OusterMsgPlugin_new,
                ::Ouster::OusterMsgPlugin_delete);
        }

        std::vector<char>& topic_type_support< ::Ouster::OusterMsg >::to_cdr_buffer(
            std::vector<char>& buffer, 
            const ::Ouster::OusterMsg& sample,
            ::dds::core::policy::DataRepresentationId representation)
        {
            // First get the length of the buffer
            unsigned int length = 0;
            RTIBool ok = OusterMsgPlugin_serialize_to_cdr_buffer(
                NULL, 
                &length,
                &sample,
                representation);
            ::rti::core::check_return_code(
                ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
                "Failed to calculate cdr buffer size");

            // Create a vector with that size and copy the cdr buffer into it
            buffer.resize(length);
            ok = OusterMsgPlugin_serialize_to_cdr_buffer(
                &buffer[0], 
                &length, 
                &sample,
                representation);
            ::rti::core::check_return_code(
                ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
                "Failed to copy cdr buffer");

            return buffer;
        }

        void topic_type_support< ::Ouster::OusterMsg >::from_cdr_buffer(::Ouster::OusterMsg& sample, 
        const std::vector<char>& buffer)
        {

            RTIBool ok  = OusterMsgPlugin_deserialize_from_cdr_buffer(
                &sample, 
                &buffer[0], 
                static_cast<unsigned int>(buffer.size()));
            ::rti::core::check_return_code(ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
            "Failed to create ::Ouster::OusterMsg from cdr buffer");
        }

        void topic_type_support< ::Ouster::OusterMsg >::reset_sample(::Ouster::OusterMsg& sample) 
        {
            sample.msg_index(0ull);
            sample.sensor_model_type(0);
            sample.return_mode(0);
            ::rti::topic::reset_sample(sample.ouster_packet());
            sample.beam_altitude_angle_type(0);
        }

        void topic_type_support< ::Ouster::OusterMsg >::allocate_sample(::Ouster::OusterMsg& sample, int, int) 
        {
            ::rti::topic::allocate_sample(sample.ouster_packet(),  600L, -1);
        }
    }
}  

#endif // NDDS_STANDALONE_TYPE
