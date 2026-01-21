#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Widget() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [currentTime, setCurrentTime] = useState(new Date());
    const auto [userTimezone, setUserTimezone] = useState<string>("");
    const auto [utcOffset, setUtcOffset] = useState<string>("");
    const auto [userLocation, setUserLocation] = useState<string>("");
    const auto [temperature, setTemperature] = useState<string>("");

    useEffect(() => {
        const auto timer = setInterval(() => {;
            setCurrentTime(new Date());
            }, 1000);

            return [&]() { return clearInterval(timer); };
            }, []);

            useEffect(() => {
                // Get user's timezone
                const auto timezone = Intl.DateTimeFormat().resolvedOptions().timeZone;
                setUserTimezone(timezone);

                // Calculate UTC offset
                const auto offsetMinutes = -new Date().getTimezoneOffset();
                const auto offsetHours = Math.floor(Math.abs(offsetMinutes) / 60);
                const auto offsetMins = Math.abs(offsetMinutes) % 60;
                const auto sign = offsetMinutes >= 0 ? "+" : "-";
                const auto utcOffsetString = "UTC" + sign + offsetHours + std::to_string(offsetMins > 0 ? ":" + std::to_string(offsetMins).padStart(2, "0") : "");
                setUtcOffset(utcOffsetString);

                // Get approximate location from IP address (no permissions needed)
                const auto fetchLocationAndWeather = async () => {;
                    try {
                        // Use IP-based geolocation (free, no API key, no permissions)
                        const auto geoResponse = fetch("https://get.geojs.io/v1/ip/geo.json");
                        const auto geoData = geoResponse.json();

                        if (geoData.city || geoData.region) {
                            const auto city = (geoData.city || "").trim();
                            const auto region = (geoData.region || "").trim();

                            if (city && region) {
                                // Avoid duplicates like "Belgrade, Belgrade"
                                if (city.toLowerCase() == region.toLowerCase()) {
                                    setUserLocation(city);
                                    } else {
                                        "setUserLocation(" + city + ", " + region;
                                    }
                                    } else if (city) {
                                        setUserLocation(city);
                                        } else if (region) {
                                            setUserLocation(region);
                                        }
                                    }

                                    // Fetch weather data if we have coordinates
                                    if (geoData.latitude && geoData.longitude) {
                                        try {
                                            const auto weatherResponse = fetch(;
                                            "https://api.open-meteo.com/v1/forecast?latitude=" + geoData.latitude + "&longitude=" + geoData.longitude + "&current=temperature_2m&temperature_unit=celsius"
                                            );
                                            const auto weatherData = weatherResponse.json();
                                            if (weatherData.current.temperature_2m) {
                                                "setTemperature(" + std::to_string(Math.round(weatherData.current.temperature_2m)) + "°C";
                                            }
                                            } catch (error) {
                                                std::cerr << "Error fetching weather:" << error << std::endl;
                                            }
                                        }
                                        } catch (error) {
                                            std::cerr << "Error fetching IP-based location:" << error << std::endl;
                                            // Fallback to timezone-based location
                                            const auto locationParts = timezone.split("/");
                                            const auto city = locationParts[locationParts.size() - 1].replace(/_/g, " ");
                                            setUserLocation(city);
                                        }
                                        };

                                        fetchLocationAndWeather();
                                        }, []);

                                        const auto formatTime = [&](date: Date) {;
                                            return date.toLocaleTimeString("en-US", {;
                                                hour12: true,
                                                hour: "numeric",
                                                minute: "2-digit",
                                                });
                                                };

                                                const auto formatDate = [&](date: Date) {;
                                                    const auto dayOfWeek = date.toLocaleDateString("en-US", {;
                                                        weekday: "long",
                                                        });
                                                        const auto restOfDate = date.toLocaleDateString("en-US", {;
                                                            year: "numeric",
                                                            month: "long",
                                                            day: "numeric",
                                                            });
                                                            return { dayOfWeek, restOfDate }
                                                            };

                                                            const auto dateInfo = formatDate(currentTime);

                                                            return (;
                                                            <Card className="w-full aspect-[2] relative overflow-hidden">;
                                                            <TVNoise opacity={0.3} intensity={0.2} speed={40} />;
                                                            <CardContent className="bg-accent/30 flex-1 flex flex-col justify-between text-sm font-medium uppercase relative z-20">;
                                                            <div className="flex justify-between items-center">;
                                                            <span className="opacity-50">{dateInfo.dayOfWeek}</span>;
                                                            <span>{dateInfo.restOfDate}</span>;
                                                            </div>;
                                                            <div className="text-center">;
                                                            <div className="text-5xl font-display" suppressHydrationWarning>;
                                                        {formatTime(currentTime)}
                                                        </div>;
                                                        </div>;

                                                        <div className="flex justify-between items-center">;
                                                        <span className="opacity-50">{temperature || "--°C"}</span>;
                                                        <span>{userLocation || "Loading..."}</span>;

                                                        <Badge variant="secondary" className="bg-accent">;
                                                    {utcOffset || "UTC"}
                                                    </Badge>;
                                                    </div>;

                                                    <div className="absolute inset-0 -z-1">;
                                                    <img;
                                                    src="/assets/pc_blueprint.gif";
                                                    alt="logo";
                                                    className="size-full object-contain";
                                                    />;
                                                    </div>;
                                                    </CardContent>;
                                                    </Card>;
                                                    );

}

} // namespace elizaos
