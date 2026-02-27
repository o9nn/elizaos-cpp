#include "florence2-model.h"

Florence2Model::Florence2Model() {
    this->localModel = std::make_shared<Florence2Local>();
}

std::shared_ptr<Promise<void>> Florence2Model::initialize()
{
    if (this->initialized) {
        return std::shared_ptr<Promise<void>>();
    }
    try
    {
        logger->info(std:("[Florence2] Initializing local Florence-2 model with TensorFlow.js..."));
        std::async([=]() { this->localModel->initialize(); });
        this->initialized = true;
        logger->info(std:("[Florence2] Local model initialized successfully"));
    }
    catch (const any& error)
    {
        logger->error(std:("[Florence2] Failed to initialize local model:"), error);
        this->initialized = true;
        logger->warn(std:("[Florence2] Running with enhanced fallback mode"));
    }
}

std::shared_ptr<Promise<std::shared_ptr<Florence2Result>>> Florence2Model::analyzeTile(std::shared_ptr<ScreenTile> tile)
{
    if (!this->initialized) {
        std::async([=]() { this->initialize(); });
    }
    if (!tile->data) {
        throw any(std::make_shared<Error>(std:("Tile has no image data")));
    }
    try
    {
        try
        {
            auto result = std::async([=]() { this->localModel->analyzeImage(tile->data); });
            logger->debug(std:("[Florence2] Analyzed tile ") + tile->id + std:(": ") + result->caption + string_empty);
            return result;
        }
        catch (const any& _modelError)
        {
            logger->warn(std:("[Florence2] Local model analysis failed, falling back:"), _modelError);
        }
        auto result = std::async([=]() { this->mockAnalyze(tile); });
        logger->debug(std:("[Florence2] Mock analyzed tile ") + tile->id + std:(": ") + result->caption + string_empty);
        return result;
    }
    catch (const any& error)
    {
        logger->error(std:("[Florence2] Analysis failed:"), error);
        throw any(error);
    }
}

std::shared_ptr<Promise<std::shared_ptr<Florence2Result>>> Florence2Model::analyzeImage(std::shared_ptr<Buffer> imageBuffer)
{
    if (!this->initialized) {
        std::async([=]() { this->initialize(); });
    }
    try
    {
        try
        {
            auto result = std::async([=]() { this->localModel->analyzeImage(imageBuffer); });
            logger->debug(std:("[Florence2] Analyzed image: ") + result->caption + string_empty);
            return result;
        }
        catch (const any& _modelError)
        {
            logger->warn(std:("[Florence2] Local model analysis failed, falling back:"), _modelError);
        }
        auto result = std::async([=]() { this->mockAnalyzeBuffer(imageBuffer); });
        logger->debug(std:("[Florence2] Mock analyzed image: ") + result->caption + string_empty);
        return result;
    }
    catch (const any& error)
    {
        logger->error(std:("[Florence2] Image analysis failed:"), error);
        throw any(error);
    }
}

std::shared_ptr<Promise<std::shared_ptr<Florence2Result>>> Florence2Model::mockAnalyze(std::shared_ptr<ScreenTile> tile)
{
    auto isUpperRegion = tile->row < 2;
    auto isLeftRegion = tile->col < 2;
    auto caption = std:("Desktop screen region");
    auto objects = array<object>();
    auto regions = array<object>();
    auto tags = array<string>();
    if (isUpperRegion) {
        caption = std:("Application window with menu bar");
        objects->push(object{
            object::pair{std:("label"), std:("window")}, 
            object::pair{std:("bbox"), object{
                object::pair{std:("x"), 0}, 
                object::pair{std:("y"), 0}, 
                object::pair{std:("width"), tile->width}, 
                object::pair{std:("height"), 50}
            }}, 
            object::pair{std:("confidence"), 0.9}
        });
        objects->push(object{
            object::pair{std:("label"), std:("menu_bar")}, 
            object::pair{std:("bbox"), object{
                object::pair{std:("x"), 0}, 
                object::pair{std:("y"), 0}, 
                object::pair{std:("width"), tile->width}, 
                object::pair{std:("height"), 30}
            }}, 
            object::pair{std:("confidence"), 0.85}
        });
        tags->push(std:("ui"), std:("application"), std:("desktop"));
    }
    if (isLeftRegion) {
        caption = std:("Sidebar or navigation area");
        objects->push(object{
            object::pair{std:("label"), std:("sidebar")}, 
            object::pair{std:("bbox"), object{
                object::pair{std:("x"), 0}, 
                object::pair{std:("y"), 0}, 
                object::pair{std:("width"), 100}, 
                object::pair{std:("height"), tile->height}
            }}, 
            object::pair{std:("confidence"), 0.8}
        });
        tags->push(std:("navigation"), std:("sidebar"));
    }
    auto buttonCount = Math->floor(Math->random() * 3) + 1;
    for (auto i = 0; i < buttonCount; i++)
    {
        objects->push(object{
            object::pair{std:("label"), std:("button")}, 
            object::pair{std:("bbox"), object{
                object::pair{std:("x"), Math->random() * (tile->width - 100)}, 
                object::pair{std:("y"), Math->random() * (tile->height - 40)}, 
                object::pair{std:("width"), 100}, 
                object::pair{std:("height"), 40}
            }}, 
            object::pair{std:("confidence"), 0.7 + Math->random() * 0.2}
        });
    }
    auto textRegions = Math->floor(Math->random() * 2) + 1;
    for (auto i = 0; i < textRegions; i++)
    {
        regions->push(object{
            object::pair{std:("description"), std:("Text content area")}, 
            object::pair{std:("bbox"), object{
                object::pair{std:("x"), Math->random() * (tile->width - 200)}, 
                object::pair{std:("y"), Math->random() * (tile->height - 100)}, 
                object::pair{std:("width"), 200}, 
                object::pair{std:("height"), 100}
            }}
        });
    }
    tags->push(std:("screen"), std:("interface"), std:("computer"));
    return object{
        object::pair{std:("caption"), std:("caption")}, 
        object::pair{std:("objects"), std:("objects")}, 
        object::pair{std:("regions"), std:("regions")}, 
        object::pair{std:("tags"), std:("tags")}
    };
}

std::shared_ptr<Promise<array<object>>> Florence2Model::detectUIElements(std::shared_ptr<Buffer> imageBuffer)
{
    if (!this->initialized) {
        std::async([=]() { this->initialize(); });
    }
    try
    {
        std::shared_ptr<Florence2Result> result;
        try
        {
            result = std::async([=]() { this->localModel->analyzeImage(imageBuffer); });
        }
        catch (const any& _modelError)
        {
            logger->warn(std:("[Florence2] Local model failed for UI detection, using fallback"));
            result = std::async([=]() { this->mockAnalyzeBuffer(imageBuffer); });
        }
        return (OR((result->objects), (array<any>())))->map([=](auto obj) mutable
        {
            return (object{
                object::pair{std:("type"), this->mapToUIElementType(obj["label"])}, 
                object::pair{std:("bbox"), obj["bbox"]}, 
                object::pair{std:("confidence"), obj["confidence"]}
            });
        }
        );
    }
    catch (const any& error)
    {
        logger->error(std:("[Florence2] UI element detection failed:"), error);
        return array<any>();
    }
}

std::shared_ptr<Promise<std::shared_ptr<Florence2Result>>> Florence2Model::mockAnalyzeBuffer(std::shared_ptr<Buffer> _imageBuffer)
{
    auto scenarios = array<object>{ object{
        object::pair{std:("caption"), std:("Indoor scene with a person in front of a computer")}, 
        object::pair{std:("objects"), array<object>{ object{
            object::pair{std:("label"), std:("person")}, 
            object::pair{std:("bbox"), object{
                object::pair{std:("x"), 300}, 
                object::pair{std:("y"), 200}, 
                object::pair{std:("width"), 200}, 
                object::pair{std:("height"), 300}
            }}, 
            object::pair{std:("confidence"), 0.9}
        }, object{
            object::pair{std:("label"), std:("computer")}, 
            object::pair{std:("bbox"), object{
                object::pair{std:("x"), 400}, 
                object::pair{std:("y"), 350}, 
                object::pair{std:("width"), 150}, 
                object::pair{std:("height"), 100}
            }}, 
            object::pair{std:("confidence"), 0.85}
        }, object{
            object::pair{std:("label"), std:("desk")}, 
            object::pair{std:("bbox"), object{
                object::pair{std:("x"), 350}, 
                object::pair{std:("y"), 400}, 
                object::pair{std:("width"), 250}, 
                object::pair{std:("height"), 100}
            }}, 
            object::pair{std:("confidence"), 0.8}
        } }}, 
        object::pair{std:("tags"), array<string>{ std:("indoor"), std:("office"), std:("workspace"), std:("person"), std:("computer") }}
    }, object{
        object::pair{std:("caption"), std:("Room interior with furniture and lighting")}, 
        object::pair{std:("objects"), array<object>{ object{
            object::pair{std:("label"), std:("chair")}, 
            object::pair{std:("bbox"), object{
                object::pair{std:("x"), 200}, 
                object::pair{std:("y"), 300}, 
                object::pair{std:("width"), 100}, 
                object::pair{std:("height"), 150}
            }}, 
            object::pair{std:("confidence"), 0.85}
        }, object{
            object::pair{std:("label"), std:("table")}, 
            object::pair{std:("bbox"), object{
                object::pair{std:("x"), 350}, 
                object::pair{std:("y"), 350}, 
                object::pair{std:("width"), 150}, 
                object::pair{std:("height"), 100}
            }}, 
            object::pair{std:("confidence"), 0.8}
        }, object{
            object::pair{std:("label"), std:("lamp")}, 
            object::pair{std:("bbox"), object{
                object::pair{std:("x"), 500}, 
                object::pair{std:("y"), 200}, 
                object::pair{std:("width"), 50}, 
                object::pair{std:("height"), 100}
            }}, 
            object::pair{std:("confidence"), 0.75}
        } }}, 
        object::pair{std:("tags"), array<string>{ std:("indoor"), std:("room"), std:("furniture"), std:("interior") }}
    }, object{
        object::pair{std:("caption"), std:("Person working at a desk with computer monitor")}, 
        object::pair{std:("objects"), array<object>{ object{
            object::pair{std:("label"), std:("person")}, 
            object::pair{std:("bbox"), object{
                object::pair{std:("x"), 250}, 
                object::pair{std:("y"), 150}, 
                object::pair{std:("width"), 250}, 
                object::pair{std:("height"), 350}
            }}, 
            object::pair{std:("confidence"), 0.92}
        }, object{
            object::pair{std:("label"), std:("monitor")}, 
            object::pair{std:("bbox"), object{
                object::pair{std:("x"), 450}, 
                object::pair{std:("y"), 300}, 
                object::pair{std:("width"), 120}, 
                object::pair{std:("height"), 80}
            }}, 
            object::pair{std:("confidence"), 0.88}
        }, object{
            object::pair{std:("label"), std:("keyboard")}, 
            object::pair{std:("bbox"), object{
                object::pair{std:("x"), 430}, 
                object::pair{std:("y"), 380}, 
                object::pair{std:("width"), 100}, 
                object::pair{std:("height"), 30}
            }}, 
            object::pair{std:("confidence"), 0.82}
        } }}, 
        object::pair{std:("tags"), array<string>{ std:("person"), std:("working"), std:("computer"), std:("desk"), std:("office") }}
    } };
    auto scenario = const_(scenarios)[Math->floor(Math->random() * scenarios->get_length())];
    return object{
        object::pair{std:("caption"), scenario["caption"]}, 
        object::pair{std:("objects"), scenario["objects"]}, 
        object::pair{std:("regions"), array<any>()}, 
        object::pair{std:("tags"), scenario["tags"]}
    };
}

string Florence2Model::mapToUIElementType(string label)
{
    auto mapping = object{
        object::pair{std:("button"), std:("button")}, 
        object::pair{std:("text_field"), std:("input")}, 
        object::pair{std:("text_area"), std:("textarea")}, 
        object::pair{std:("checkbox"), std:("checkbox")}, 
        object::pair{std:("radio_button"), std:("radio")}, 
        object::pair{std:("dropdown"), std:("select")}, 
        object::pair{std:("menu"), std:("menu")}, 
        object::pair{std:("menu_bar"), std:("menubar")}, 
        object::pair{std:("toolbar"), std:("toolbar")}, 
        object::pair{std:("window"), std:("window")}, 
        object::pair{std:("dialog"), std:("dialog")}, 
        object::pair{std:("icon"), std:("icon")}, 
        object::pair{std:("image"), std:("image")}, 
        object::pair{std:("video"), std:("video")}, 
        object::pair{std:("link"), std:("link")}, 
        object::pair{std:("heading"), std:("heading")}, 
        object::pair{std:("paragraph"), std:("text")}, 
        object::pair{std:("list"), std:("list")}, 
        object::pair{std:("table"), std:("table")}, 
        object::pair{std:("scrollbar"), std:("scrollbar")}, 
        object::pair{std:("tab"), std:("tab")}, 
        object::pair{std:("panel"), std:("panel")}
    };
    return OR((const_(mapping)[label->toLowerCase()]), (std:("unknown")));
}

std::shared_ptr<Promise<object>> Florence2Model::generateSceneGraph(array<std::shared_ptr<ScreenTile>> tiles)
{
    auto nodes = array<object>();
    auto edges = array<object>();
    auto& __array9145_9817 = tiles;
    for (auto __indx9145_9817 = 0_N; __indx9145_9817 < __array9145_9817->get_length(); __indx9145_9817++)
    {
        auto& tile = const_(__array9145_9817)[__indx9145_9817];
        {
            if (!tile->data) {
                continue;
            }
            auto analysis = std::async([=]() { this->analyzeTile(tile); });
            if (analysis->objects) {
                auto& __array9368_9803 = analysis->objects;
                for (auto __indx9368_9803 = 0_N; __indx9368_9803 < __array9368_9803->get_length(); __indx9368_9803++)
                {
                    auto& obj = const_(__array9368_9803)[__indx9368_9803];
                    {
                        auto nodeId = string_empty + tile->id + std:("-") + obj["label"] + std:("-") + nodes->get_length() + string_empty;
                        nodes->push(object{
                            object::pair{std:("id"), nodeId}, 
                            object::pair{std:("type"), obj["label"]}, 
                            object::pair{std:("label"), obj["label"]}, 
                            object::pair{std:("position"), object{
                                object::pair{std:("x"), tile->x + obj["bbox"]->x}, 
                                object::pair{std:("y"), tile->y + obj["bbox"]->y}, 
                                object::pair{std:("width"), obj["bbox"]->width}, 
                                object::pair{std:("height"), obj["bbox"]->height}
                            }}
                        });
                    }
                }
            }
        }
    }
    for (auto i = 0; i < nodes->get_length(); i++)
    {
        for (auto j = i + 1; j < nodes->get_length(); j++)
        {
            auto relation = this->inferSpatialRelation(const_(nodes)[i]["position"], const_(nodes)[j]["position"]);
            if (relation) {
                edges->push(object{
                    object::pair{std:("source"), const_(nodes)[i]["id"]}, 
                    object::pair{std:("target"), const_(nodes)[j]["id"]}, 
                    object::pair{std:("relation"), std:("relation")}
                });
            }
        }
    }
    return object{
        object::pair{std:("nodes"), std:("nodes")}, 
        object::pair{std:("edges"), std:("edges")}
    };
}

any Florence2Model::inferSpatialRelation(std::shared_ptr<BoundingBox> box1, std::shared_ptr<BoundingBox> box2)
{
    auto center1 = object{
        object::pair{std:("x"), box1->x + box1->width / 2}, 
        object::pair{std:("y"), box1->y + box1->height / 2}
    };
    auto center2 = object{
        object::pair{std:("x"), box2->x + box2->width / 2}, 
        object::pair{std:("y"), box2->y + box2->height / 2}
    };
    if (this->contains(box1, box2)) {
        return std:("contains");
    }
    if (this->contains(box2, box1)) {
        return std:("contained_by");
    }
    if (this->overlaps(box1, box2)) {
        return std:("overlaps");
    }
    auto dx = center2["x"] - center1["x"];
    auto dy = center2["y"] - center1["y"];
    auto distance = Math->sqrt(dx * dx + dy * dy);
    if (distance < 100) {
        if (Math->abs(dx) > Math->abs(dy)) {
            return (dx > 0) ? std:("right_of") : std:("left_of");
        } else {
            return (dy > 0) ? std:("below") : std:("above");
        }
    }
    return nullptr;
}

boolean Florence2Model::contains(std::shared_ptr<BoundingBox> box1, std::shared_ptr<BoundingBox> box2)
{
    return (AND((AND((AND((box1->x <= box2->x), (box1->y <= box2->y))), (box1->x + box1->width >= box2->x + box2->width))), (box1->y + box1->height >= box2->y + box2->height)));
}

boolean Florence2Model::overlaps(std::shared_ptr<BoundingBox> box1, std::shared_ptr<BoundingBox> box2)
{
    return !(OR((OR((OR((box1->x + box1->width < box2->x), (box2->x + box2->width < box1->x))), (box1->y + box1->height < box2->y))), (box2->y + box2->height < box1->y)));
}

boolean Florence2Model::isInitialized()
{
    return this->initialized;
}

std::shared_ptr<Promise<void>> Florence2Model::dispose()
{
    this->initialized = false;
    logger->info(std:("[Florence2] Model disposed"));
    return std::shared_ptr<Promise<void>>();
}

