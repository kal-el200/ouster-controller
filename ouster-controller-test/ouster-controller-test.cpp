// ouster-controller-test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <OusterController.h>
#include <Callback.h>
#include <ouster_dds_model.hpp>

using namespace Ouster;

static void notified(const OusterMsg& msg)
{
    std::cout << "Msg Arrived - " << msg.msg_index() << std::endl;
}

//int main(int argc, char* argv[])
//{
//	if (argc < 2)
//	{
//		std::cerr << "Usage: " << argv[0] << " <config.json>" << std::endl;
//		return EXIT_FAILURE;
//	}
//
//	auto& controller = OusterController::getInstance();
//	controller.registerOusterMsgCallback(notified);
//	return controller.run(argv[1]) ? EXIT_SUCCESS : EXIT_FAILURE;
//}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <config.json>" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Getting controller instance..." << std::endl;
    auto& controller = OusterController::getInstance();

    std::cout << "Registering callback..." << std::endl;
    std::function<void(const OusterMsg&)> callback = notified; // Create std::function explicitly
    controller.registerOusterMsgCallback(std::move(callback));

    std::cout << "Starting controller..." << std::endl;
    bool result = controller.run(argv[1]);
    std::cout << "Controller run result: " << (result ? "success" : "failure") << std::endl;

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
