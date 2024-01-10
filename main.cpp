#include "HDVR.h"
#include "Model.h"
#include "hype/Utils.h"

#define MEMORY_PATH             "./memory"
#define DATASET_PATH            "./dataset"
#define MEMORY_DATASET_PATH     "./memory/dataset"

using namespace hype;
using namespace hdvr;

int main() {
    const int frequency_points = 617;
    const int level = 10;
    const int dimensions = 10000;
    const int epochs = 2;
    const SeedingStrategy seedingStrategy = POLAR;

    Model<level, dimensions, frequency_points, seedingStrategy> model;
    HDVR hdvr(model);

    if (!model.load(MEMORY_PATH)) {
        hype::log_info_nl("No model could be loaded; continuing with untrained model.");
    }
    if (!hdvr.load_datasets(MEMORY_DATASET_PATH)) {
        hdvr.load_datasets(DATASET_PATH);
        hdvr.save_datasets(MEMORY_DATASET_PATH);
    }
    Metrics metrics = hdvr.train(epochs);
    log_info_nl("=== SUCCESS ===");

    metrics.save("./experiments", "experiment");
    model.save(MEMORY_PATH);

    return 0;
}
