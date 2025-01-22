#ifndef CONFIG_H
#define CONFIG_H

#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <stdexcept>
#include <iostream>

using json = nlohmann::json;


// JSON serialization for Eigen::Matrix4d
namespace nlohmann
{
	template<>
	struct adl_serializer<Eigen::Matrix4d>
	{
		static void to_json(json& js, const Eigen::Matrix4d& m)
		{
			std::vector<double> data;
			data.reserve(16);

			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					data.push_back(m(i, j));

			js = data;
		}

		static void from_json(const json& js, Eigen::Matrix4d& m)
		{
			auto data = js.get<std::vector<double>>();

			if (data.size() != 16)
			{
				throw std::runtime_error("Matrix data must have 16 elements");
			}

			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					m(i, j) = data[i * 4 + j];
		}
	};
}

class OusterConfiguration
{
private:
	std::vector<double> calculated_altitude_angles_;
	std::vector<double> calculated_azimuth_angles_;
	bool angles_calculated_ = false;

	void calculate_beam_angles()
	{
		if (angles_calculated_) return;

		auto alt_range = get<json>("sensor.beam_geometry.altitude_range");
		auto az_range = get<json>("sensor.beam_geometry.azimuth_range");

		// Calculate altitude angles
		double alt_min = alt_range["min"].get<double>();
		double alt_max = alt_range["max"].get<double>();
		int num_beams = alt_range["num_beams"].get<int>();

		calculated_altitude_angles_.resize(num_beams);
		if (num_beams > 1)
		{
			double alt_step = (alt_max - alt_min) / (num_beams - 1);
			for (int i = 0; i < num_beams; i++)
			{
				calculated_altitude_angles_[i] = alt_min + i * alt_step;
			}
		}

		// Calculate azimuth angles
		double az_min = az_range["min"].get<double>();
		double az_max = az_range["max"].get<double>();
		int cols_per_frame = az_range["columns_per_frame"].get<int>();

		calculated_azimuth_angles_.resize(cols_per_frame);
		if (cols_per_frame > 1)
		{
			double az_step = (az_max - az_min) / cols_per_frame;
			for (int i = 0; i < cols_per_frame; i++)
			{
				calculated_azimuth_angles_[i] = az_min + i * az_step;
			}
		}

		angles_calculated_ = true;
	}

public:
	static OusterConfiguration& getInstance()
	{
		static OusterConfiguration instance;
		return instance;
	}

	const std::vector<double>& get_altitude_angles()
	{
		if (!angles_calculated_) calculate_beam_angles();
		return calculated_altitude_angles_;
	}

	const std::vector<double>& get_azimuth_angles()
	{
		if (!angles_calculated_) calculate_beam_angles();
		return calculated_azimuth_angles_;
	}

	void load(const std::string& filepath)
	{
		try
		{
			std::ifstream file(filepath);
			if (!file.is_open())
			{
				throw std::runtime_error("Unable to open config file: " + filepath);
			}
			config = nlohmann::json::parse(file, nullptr, true, true);
		}
		catch (const nlohmann::json::parse_error& e)
		{
			throw std::runtime_error("Config parse error: " + std::string(e.what()));
		}
	}

	template<typename T>
	T get(const std::string& key) const
	{
		if (key.find('.') != std::string::npos)
		{
			// Handle nested keys (e.g., "database.host")
			nlohmann::json current = config;
			size_t start = 0;
			size_t end = key.find('.');

			while (end != std::string::npos)
			{
				std::string segment = key.substr(start, end - start);
				if (!current.contains(segment))
				{
					throw std::runtime_error("No value was found in configuration for " + key);
				}
				current = current[segment];
				start = end + 1;
				end = key.find('.', start);
			}

			std::string lastKey = key.substr(start);
			if (!current.contains(lastKey))
			{
				throw std::runtime_error("No value was found in configuration for " + key);
			}
			return current[lastKey].get<T>();
		}

		return config.at(key);
	}

	bool exists(const std::string& key) const
	{
		try
		{
			if (key.find('.') != std::string::npos)
			{
				// Handle nested keys
				nlohmann::json current = config;
				size_t start = 0;
				size_t end = key.find('.');

				while (end != std::string::npos)
				{
					std::string segment = key.substr(start, end - start);
					if (!current.contains(segment))
					{
						return false;
					}
					current = current[segment];
					start = end + 1;
					end = key.find('.', start);
				}

				return current.contains(key.substr(start));
			}

			return config.contains(key);
		}
		catch (const nlohmann::json::exception&)
		{
			return false;
		}
	}

	void save(const std::string& filepath) const
	{
		std::ofstream file(filepath);
		if (!file.is_open())
		{
			throw std::runtime_error("Unable to open file for writing: " + filepath);
		}
		file << std::setw(4) << config << std::endl;
	}

private:
	OusterConfiguration() = default;
	nlohmann::json config;

public:
	OusterConfiguration(const OusterConfiguration&) = delete;
	OusterConfiguration& operator=(const OusterConfiguration&) = delete;
};

#endif // CONFIG_H

