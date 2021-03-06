#include "mnist_benchmark.hpp"

#include <ocv_libs/tiny_cnn/trainer.hpp>

#include <boost/progress.hpp>

#include <tiny_cnn/tiny_cnn.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <fstream>
#include <iostream>

using namespace tiny_cnn;
using namespace tiny_cnn::activation;

mnist_benchmark::mnist_benchmark()
{
    network<mse, adagrad> nn; // specify loss-function and learning strategy

    using activate = relu;
    nn << convolutional_layer<activate>(32, 32, 5, 1, 6, padding::same)
       << max_pooling_layer<activate>(32, 32, 6, 2)
       << convolutional_layer<activate>(16, 16, 5, 6, 16, padding::same)
       << max_pooling_layer<activate>(16, 16, 16, 2)
       << convolutional_layer<activate>(8, 8, 5, 16, 8)
       << fully_connected_layer<activate>(128, 10);    

    std::cout << "load models..." << std::endl;

    // load MNIST dataset
    std::vector<label_t> train_labels, test_labels;
    std::vector<vec_t> train_images, test_images;

    parse_mnist_labels("mnist/train-labels.idx1-ubyte", &train_labels);
    parse_mnist_images("mnist/train-images.idx3-ubyte", &train_images, -1.0, 1.0, 2, 2);
    parse_mnist_labels("mnist/t10k-labels.idx1-ubyte", &test_labels);
    parse_mnist_images("mnist/t10k-images.idx3-ubyte", &test_images, -1.0, 1.0, 2, 2);

    std::cout << "start learning" << std::endl;

    int const minibatch_size = 10;
    int const num_epochs = 30;

    nn.optimizer().alpha *= std::sqrt(minibatch_size);

    ocv::tiny_cnn::trainer tt("LeNet-weights", minibatch_size, num_epochs);
    tt.train_and_test(nn, train_images, train_labels,
                      test_images, test_labels);
}
