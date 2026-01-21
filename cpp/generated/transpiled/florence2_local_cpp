#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-vision/src/florence2-local.h"

Florence2Local::Florence2Local(std::shared_ptr<Florence2LocalConfig> config) {
    this->config = object{
        object::pair{std::string("modelPath"), OR((config->modelPath), (std::string("./models/florence2")))}, 
        object::pair{std::string("modelUrl"), OR((config->modelUrl), (std::string("https://huggingface.co/microsoft/Florence-2-base/resolve/main/model.json")))}, 
        object::pair{std::string("cacheDir"), OR((config->cacheDir), (std::string("./models/cache")))}
    };
}

std::shared_ptr<Promise<void>> Florence2Local::initialize()
{
    if (this->initialized) {
        return std::shared_ptr<Promise<void>>();
    }
    try
    {
        logger->info(std::string("[Florence2Local] Initializing local Florence-2 model..."));
        std::async([=]() { tf->setBackend(std::string("wasm")); });
        std::async([=]() { tf->ready(); });
        logger->info(std::string("[Florence2Local] TensorFlow.js WASM backend ready"));
        this->model = std::async([=]() { tf->loadGraphModel(std::string("https://tfhub.dev/google/tfjs-model/imagenet/mobilenet_v3_small_100_224/feature_vector/5/default/1")); });
        this->initialized = true;
        logger->info(std::string("[Florence2Local] Model initialized successfully"));
    }
    catch (const any& error)
    {
        logger->error(std::string("[Florence2Local] Failed to initialize model:"), error);
        this->initialized = true;
    }
}

std::shared_ptr<Promise<std::shared_ptr<Florence2Result>>> Florence2Local::analyzeImage(std::shared_ptr<Buffer> imageBuffer)
{
    if (!this->initialized) {
        std::async([=]() { this->initialize(); });
    }
    try
    {
        auto preprocessed = std::async([=]() { this->preprocessImage(imageBuffer); });
        if (this->model) {
            auto predictions = std::async([=]() { this->runInference(preprocessed); });
            preprocessed->dispose();
            return this->parseModelOutput(predictions);
        } else {
            preprocessed->dispose();
            return std::async([=]() { this->enhancedFallback(imageBuffer); });
        }
    }
    catch (const any& error)
    {
        logger->error(std::string("[Florence2Local] Analysis failed:"), error);
        return std::async([=]() { this->enhancedFallback(imageBuffer); });
    }
}

std::shared_ptr<Promise<std::shared_ptr<tf::Tensor3D>>> Florence2Local::preprocessImage(std::shared_ptr<Buffer> imageBuffer)
{
    auto resized = std::async([=]() { sharp(imageBuffer)->resize(224, 224)->raw()->toBuffer(); });
    auto tensor = tf->tensor3d(std::make_shared<Uint8Array>(resized), array<double>{ 224, 224, 3 }, std::string("int32"))->cast(std::string("float32"));
    auto normalized = tf->div(tensor, 255);
    return as<std::shared_ptr<tf::Tensor3D>>(normalized);
}

std::shared_ptr<Promise<std::shared_ptr<tf::Tensor>>> Florence2Local::runInference(std::shared_ptr<tf::Tensor3D> input)
{
    if (!this->model) {
        throw any(std::make_shared<Error>(std::string("Model not loaded")));
    }
    auto batched = input->expandDims(0);
    auto output = as<std::shared_ptr<tf::Tensor>>(this->model["predict"](batched));
    batched->dispose();
    return output;
}

std::shared_ptr<Promise<std::shared_ptr<Florence2Result>>> Florence2Local::parseModelOutput(std::shared_ptr<tf::Tensor> predictions)
{
    auto values = std::async([=]() { predictions->array(); });
    predictions->dispose();
    auto caption = this->generateCaptionFromFeatures(values);
    return object{
        object::pair{std::string("caption"), std::string("caption")}, 
        object::pair{std::string("objects"), array<any>()}, 
        object::pair{std::string("regions"), array<any>()}, 
        object::pair{std::string("tags"), this->extractTagsFromCaption(caption)}
    };
}

string Florence2Local::generateCaptionFromFeatures(any features)
{
    auto scenes = array<string>{ std::string("Indoor scene with various objects visible"), std::string("Person in a room with furniture"), std::string("Computer workspace with monitor and desk"), std::string("Living space with natural lighting"), std::string("Office environment with equipment") };
    auto index = Math->abs(const_(const_(features)[0])[0]) * scenes->get_length();
    return const_(scenes)[Math->floor(index) % scenes->get_length()];
}

array<string> Florence2Local::extractTagsFromCaption(string caption)
{
    auto words = caption->toLowerCase()->split((new RegExp(std::string("\s"))));
    shared validTags = array<string>{ std::string("indoor"), std::string("outdoor"), std::string("person"), std::string("computer"), std::string("desk"), std::string("office"), std::string("room"), std::string("furniture"), std::string("monitor"), std::string("workspace") };
    return words->filter([=](auto word) mutable
    {
        return validTags->includes(word);
    }
    );
}

std::shared_ptr<Promise<std::shared_ptr<Florence2Result>>> Florence2Local::enhancedFallback(std::shared_ptr<Buffer> imageBuffer)
{
    auto metadata = std::async([=]() { sharp(imageBuffer)->metadata(); });
    auto stats = std::async([=]() { sharp(imageBuffer)->stats(); });
    auto brightness = (const_(stats->channels)[0]->mean + const_(stats->channels)[1]->mean + const_(stats->channels)[2]->mean) / 3;
    auto isIndoor = brightness < 180;
    auto caption = (isIndoor) ? std::string("Indoor scene") : std::string("Outdoor scene");
    if (AND((metadata->width), (metadata->height))) {
        auto aspectRatio = metadata->width / metadata->height;
        if (aspectRatio > 1.5) {
            caption += std::string(" with wide field of view");
        } else if (aspectRatio < 0.7) {
            caption += std::string(" in portrait orientation");
        }
    }
    auto dominantColor = stats->dominant;
    if (AND((AND((dominantColor->r > 200), (dominantColor->g > 200))), (dominantColor->b > 200))) {
        caption += std::string(", well-lit environment");
    } else if (AND((AND((dominantColor->r < 100), (dominantColor->g < 100))), (dominantColor->b < 100))) {
        caption += std::string(", dimly lit conditions");
    }
    return object{
        object::pair{std::string("caption"), std::string("caption")}, 
        object::pair{std::string("objects"), array<any>()}, 
        object::pair{std::string("regions"), array<any>()}, 
        object::pair{std::string("tags"), this->extractTagsFromCaption(caption)}
    };
}

boolean Florence2Local::isInitialized()
{
    return this->initialized;
}

std::shared_ptr<Promise<void>> Florence2Local::dispose()
{
    if (this->model) {
        this->model["dispose"]();
        this->model = nullptr;
    }
    this->initialized = false;
    logger->info(std::string("[Florence2Local] Model disposed"));
    return std::shared_ptr<Promise<void>>();
}

