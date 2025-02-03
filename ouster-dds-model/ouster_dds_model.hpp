

/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from ouster_dds_model.idl
using RTI Code Generator (rtiddsgen) version 4.3.0.
The rtiddsgen tool is part of the RTI Connext DDS distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the Code Generator User's Manual.
*/

#ifndef ouster_dds_model_1522706435_hpp
#define ouster_dds_model_1522706435_hpp

#include <iosfwd>

#if (defined(RTI_WIN32) || defined (RTI_WINCE) || defined(RTI_INTIME)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols.
*/
#undef RTIUSERDllExport
#define RTIUSERDllExport __declspec(dllexport)
#endif

#include "dds/core/SafeEnumeration.hpp"
#include "dds/core/String.hpp"
#include "dds/core/array.hpp"
#include "dds/core/vector.hpp"
#include "dds/core/External.hpp"
#include "rti/core/LongDouble.hpp"
#include "rti/core/Pointer.hpp"
#include "rti/core/array.hpp"
#include "rti/topic/TopicTraits.hpp"

#include "omg/types/string_view.hpp"

#include "rti/core/BoundedSequence.hpp"
#include "dds/core/Optional.hpp"

#ifndef NDDS_STANDALONE_TYPE
#include "dds/domain/DomainParticipant.hpp"
#include "dds/topic/TopicTraits.hpp"
#include "dds/core/xtypes/DynamicType.hpp"
#include "dds/core/xtypes/StructType.hpp"
#include "dds/core/xtypes/UnionType.hpp"
#include "dds/core/xtypes/EnumType.hpp"
#include "dds/core/xtypes/AliasType.hpp"
#include "rti/util/StreamFlagSaver.hpp"
#include "rti/domain/PluginSupport.hpp"
#endif

#if (defined(RTI_WIN32) || defined (RTI_WINCE) || defined(RTI_INTIME)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, stop exporting symbols.
*/
#undef RTIUSERDllExport
#define RTIUSERDllExport
#endif

#if (defined(RTI_WIN32) || defined (RTI_WINCE) || defined(RTI_INTIME)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport __declspec(dllexport)
#endif

namespace Ouster {

    class NDDSUSERDllExport OusterRayData {
      public:

        OusterRayData();

        OusterRayData(uint16_t distance_first_,uint8_t reflectivity_first_,uint16_t nir_value_);

        uint16_t& distance_first() noexcept {
            return m_distance_first_;
        }

        const uint16_t& distance_first() const noexcept {
            return m_distance_first_;
        }

        void distance_first(uint16_t value) {

            m_distance_first_ = value;
        }

        uint8_t& reflectivity_first() noexcept {
            return m_reflectivity_first_;
        }

        const uint8_t& reflectivity_first() const noexcept {
            return m_reflectivity_first_;
        }

        void reflectivity_first(uint8_t value) {

            m_reflectivity_first_ = value;
        }

        uint16_t& nir_value() noexcept {
            return m_nir_value_;
        }

        const uint16_t& nir_value() const noexcept {
            return m_nir_value_;
        }

        void nir_value(uint16_t value) {

            m_nir_value_ = value;
        }

        bool operator == (const OusterRayData& other_) const;
        bool operator != (const OusterRayData& other_) const;

        void swap(OusterRayData& other_) noexcept ;

      private:

        uint16_t m_distance_first_;
        uint8_t m_reflectivity_first_;
        uint16_t m_nir_value_;

    };

    inline void swap(OusterRayData& a, OusterRayData& b)  noexcept 
    {
        a.swap(b);
    }

    NDDSUSERDllExport std::ostream& operator<<(std::ostream& o, const OusterRayData& sample);

    #if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
    // On Windows, dll-export template instantiations of standard types used by
    // other dll-exported types
    template class NDDSUSERDllExport std::allocator< ::Ouster::OusterRayData >;
    template class NDDSUSERDllExport std::vector< ::Ouster::OusterRayData >;
    template class NDDSUSERDllExport std::allocator< uint8_t >;
    template class NDDSUSERDllExport std::vector< uint8_t >;
    template class NDDSUSERDllExport std::allocator< uint16_t >;
    template class NDDSUSERDllExport std::vector< uint16_t >;
    #endif
    class NDDSUSERDllExport OusterSingleFiring {
      public:

        OusterSingleFiring();

        OusterSingleFiring(uint16_t rotational_direction_azimuth_,const ::rti::core::bounded_sequence< ::Ouster::OusterRayData, 128L >& ouster_ray_data_,const ::rti::core::bounded_sequence< uint8_t, 128L >& reflectivity_second_,const ::rti::core::bounded_sequence< uint16_t, 128L >& distance_second_,int64_t azimuth_firing_time_);

        uint16_t& rotational_direction_azimuth() noexcept {
            return m_rotational_direction_azimuth_;
        }

        const uint16_t& rotational_direction_azimuth() const noexcept {
            return m_rotational_direction_azimuth_;
        }

        void rotational_direction_azimuth(uint16_t value) {

            m_rotational_direction_azimuth_ = value;
        }

        ::rti::core::bounded_sequence< ::Ouster::OusterRayData, 128L >& ouster_ray_data() noexcept {
            return m_ouster_ray_data_;
        }

        const ::rti::core::bounded_sequence< ::Ouster::OusterRayData, 128L >& ouster_ray_data() const noexcept {
            return m_ouster_ray_data_;
        }

        void ouster_ray_data(const ::rti::core::bounded_sequence< ::Ouster::OusterRayData, 128L >& value) {

            m_ouster_ray_data_ = value;
        }

        void ouster_ray_data(::rti::core::bounded_sequence< ::Ouster::OusterRayData, 128L >&& value) {
            m_ouster_ray_data_ = std::move(value);
        }
        ::rti::core::bounded_sequence< uint8_t, 128L >& reflectivity_second() noexcept {
            return m_reflectivity_second_;
        }

        const ::rti::core::bounded_sequence< uint8_t, 128L >& reflectivity_second() const noexcept {
            return m_reflectivity_second_;
        }

        void reflectivity_second(const ::rti::core::bounded_sequence< uint8_t, 128L >& value) {

            m_reflectivity_second_ = value;
        }

        void reflectivity_second(::rti::core::bounded_sequence< uint8_t, 128L >&& value) {
            m_reflectivity_second_ = std::move(value);
        }
        ::rti::core::bounded_sequence< uint16_t, 128L >& distance_second() noexcept {
            return m_distance_second_;
        }

        const ::rti::core::bounded_sequence< uint16_t, 128L >& distance_second() const noexcept {
            return m_distance_second_;
        }

        void distance_second(const ::rti::core::bounded_sequence< uint16_t, 128L >& value) {

            m_distance_second_ = value;
        }

        void distance_second(::rti::core::bounded_sequence< uint16_t, 128L >&& value) {
            m_distance_second_ = std::move(value);
        }
        int64_t& azimuth_firing_time() noexcept {
            return m_azimuth_firing_time_;
        }

        const int64_t& azimuth_firing_time() const noexcept {
            return m_azimuth_firing_time_;
        }

        void azimuth_firing_time(int64_t value) {

            m_azimuth_firing_time_ = value;
        }

        bool operator == (const OusterSingleFiring& other_) const;
        bool operator != (const OusterSingleFiring& other_) const;

        void swap(OusterSingleFiring& other_) noexcept ;

      private:

        uint16_t m_rotational_direction_azimuth_;
        ::rti::core::bounded_sequence< ::Ouster::OusterRayData, 128L > m_ouster_ray_data_;
        ::rti::core::bounded_sequence< uint8_t, 128L > m_reflectivity_second_;
        ::rti::core::bounded_sequence< uint16_t, 128L > m_distance_second_;
        int64_t m_azimuth_firing_time_;

    };

    inline void swap(OusterSingleFiring& a, OusterSingleFiring& b)  noexcept 
    {
        a.swap(b);
    }

    NDDSUSERDllExport std::ostream& operator<<(std::ostream& o, const OusterSingleFiring& sample);

    #if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
    // On Windows, dll-export template instantiations of standard types used by
    // other dll-exported types
    template class NDDSUSERDllExport std::allocator< ::Ouster::OusterSingleFiring >;
    template class NDDSUSERDllExport std::vector< ::Ouster::OusterSingleFiring >;
    #endif
    class NDDSUSERDllExport OusterMsg {
      public:

        OusterMsg();

        OusterMsg(uint64_t msg_index_,int8_t sensor_model_type_,int8_t return_mode_,const ::rti::core::bounded_sequence< ::Ouster::OusterSingleFiring, 600L >& ouster_packet_,uint8_t beam_altitude_angle_type_);

        uint64_t& msg_index() noexcept {
            return m_msg_index_;
        }

        const uint64_t& msg_index() const noexcept {
            return m_msg_index_;
        }

        void msg_index(uint64_t value) {

            m_msg_index_ = value;
        }

        int8_t& sensor_model_type() noexcept {
            return m_sensor_model_type_;
        }

        const int8_t& sensor_model_type() const noexcept {
            return m_sensor_model_type_;
        }

        void sensor_model_type(int8_t value) {

            m_sensor_model_type_ = value;
        }

        int8_t& return_mode() noexcept {
            return m_return_mode_;
        }

        const int8_t& return_mode() const noexcept {
            return m_return_mode_;
        }

        void return_mode(int8_t value) {

            m_return_mode_ = value;
        }

        ::rti::core::bounded_sequence< ::Ouster::OusterSingleFiring, 600L >& ouster_packet() noexcept {
            return m_ouster_packet_;
        }

        const ::rti::core::bounded_sequence< ::Ouster::OusterSingleFiring, 600L >& ouster_packet() const noexcept {
            return m_ouster_packet_;
        }

        void ouster_packet(const ::rti::core::bounded_sequence< ::Ouster::OusterSingleFiring, 600L >& value) {

            m_ouster_packet_ = value;
        }

        void ouster_packet(::rti::core::bounded_sequence< ::Ouster::OusterSingleFiring, 600L >&& value) {
            m_ouster_packet_ = std::move(value);
        }
        uint8_t& beam_altitude_angle_type() noexcept {
            return m_beam_altitude_angle_type_;
        }

        const uint8_t& beam_altitude_angle_type() const noexcept {
            return m_beam_altitude_angle_type_;
        }

        void beam_altitude_angle_type(uint8_t value) {

            m_beam_altitude_angle_type_ = value;
        }

        bool operator == (const OusterMsg& other_) const;
        bool operator != (const OusterMsg& other_) const;

        void swap(OusterMsg& other_) noexcept ;

      private:

        uint64_t m_msg_index_;
        int8_t m_sensor_model_type_;
        int8_t m_return_mode_;
        ::rti::core::bounded_sequence< ::Ouster::OusterSingleFiring, 600L > m_ouster_packet_;
        uint8_t m_beam_altitude_angle_type_;

    };

    inline void swap(OusterMsg& a, OusterMsg& b)  noexcept 
    {
        a.swap(b);
    }

    NDDSUSERDllExport std::ostream& operator<<(std::ostream& o, const OusterMsg& sample);

} // namespace Ouster  

#ifdef NDDS_STANDALONE_TYPE
namespace rti { 
    namespace topic {
    }
}
#else

namespace rti {
    namespace flat {
        namespace topic {
        }
    }
}
namespace dds {
    namespace topic {

        template<>
        struct topic_type_name< ::Ouster::OusterRayData > {
            NDDSUSERDllExport static std::string value() {
                return "Ouster::OusterRayData";
            }
        };

        template<>
        struct topic_type_support< ::Ouster::OusterRayData > {
            NDDSUSERDllExport 
            static void reset_sample(::Ouster::OusterRayData& sample);

            NDDSUSERDllExport 
            static void allocate_sample(::Ouster::OusterRayData& sample, int, int);

            static const ::rti::topic::TypePluginKind::type type_plugin_kind = 
            ::rti::topic::TypePluginKind::STL;
        };
        template<>
        struct topic_type_name< ::Ouster::OusterSingleFiring > {
            NDDSUSERDllExport static std::string value() {
                return "Ouster::OusterSingleFiring";
            }
        };

        template<>
        struct topic_type_support< ::Ouster::OusterSingleFiring > {
            NDDSUSERDllExport 
            static void reset_sample(::Ouster::OusterSingleFiring& sample);

            NDDSUSERDllExport 
            static void allocate_sample(::Ouster::OusterSingleFiring& sample, int, int);

            static const ::rti::topic::TypePluginKind::type type_plugin_kind = 
            ::rti::topic::TypePluginKind::STL;
        };
        template<>
        struct topic_type_name< ::Ouster::OusterMsg > {
            NDDSUSERDllExport static std::string value() {
                return "Ouster::OusterMsg";
            }
        };

        template<>
        struct is_topic_type< ::Ouster::OusterMsg > : public ::dds::core::true_type {};

        template<>
        struct topic_type_support< ::Ouster::OusterMsg > {
            NDDSUSERDllExport 
            static void register_type(
                ::dds::domain::DomainParticipant& participant,
                const std::string & type_name);

            NDDSUSERDllExport 
            static std::vector<char>& to_cdr_buffer(
                std::vector<char>& buffer, 
                const ::Ouster::OusterMsg& sample,
                ::dds::core::policy::DataRepresentationId representation 
                = ::dds::core::policy::DataRepresentation::auto_id());

            NDDSUSERDllExport 
            static void from_cdr_buffer(::Ouster::OusterMsg& sample, const std::vector<char>& buffer);
            NDDSUSERDllExport 
            static void reset_sample(::Ouster::OusterMsg& sample);

            NDDSUSERDllExport 
            static void allocate_sample(::Ouster::OusterMsg& sample, int, int);

            static const ::rti::topic::TypePluginKind::type type_plugin_kind = 
            ::rti::topic::TypePluginKind::STL;
        };
    }
}

namespace rti { 
    namespace topic {

        template<>
        struct dynamic_type< ::Ouster::OusterRayData > {
            typedef ::dds::core::xtypes::StructType type;
            NDDSUSERDllExport static const ::dds::core::xtypes::StructType& get();
        };

        template <>
        struct extensibility< ::Ouster::OusterRayData > {
            static const ::dds::core::xtypes::ExtensibilityKind::type kind =
            ::dds::core::xtypes::ExtensibilityKind::EXTENSIBLE;    };

        template<>
        struct dynamic_type< ::Ouster::OusterSingleFiring > {
            typedef ::dds::core::xtypes::StructType type;
            NDDSUSERDllExport static const ::dds::core::xtypes::StructType& get();
        };

        template <>
        struct extensibility< ::Ouster::OusterSingleFiring > {
            static const ::dds::core::xtypes::ExtensibilityKind::type kind =
            ::dds::core::xtypes::ExtensibilityKind::EXTENSIBLE;    };

        template<>
        struct dynamic_type< ::Ouster::OusterMsg > {
            typedef ::dds::core::xtypes::StructType type;
            NDDSUSERDllExport static const ::dds::core::xtypes::StructType& get();
        };

        template <>
        struct extensibility< ::Ouster::OusterMsg > {
            static const ::dds::core::xtypes::ExtensibilityKind::type kind =
            ::dds::core::xtypes::ExtensibilityKind::EXTENSIBLE;    };

    }
}

#endif // NDDS_STANDALONE_TYPE
#if (defined(RTI_WIN32) || defined (RTI_WINCE) || defined(RTI_INTIME)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, stop exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport
#endif

#endif // ouster_dds_model_1522706435_hpp

