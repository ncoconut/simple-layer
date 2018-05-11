// UMGIV
// Testing a layer creation API for costmap_2d in ROS.
// Kushal Jaligama

#include <costmap_2d/costmap_2d_ros.h>
#include <ros/ros.h>
#include <simple_layer/grid_layer.h>
#include <simple_layer/simple_layer.h>
#include <tf/transform_listener.h>
#include <vector>

int main(int argc, char* argv[]) {
  ros::init(argc, argv, "simple_layer_node");
  ros::NodeHandle nh("simple_layer_node");

  tf::TransformListener tf(ros::Duration(10));
  costmap_2d::Costmap2DROS master_grid("umigv_costmap", tf);
  // ros::Publisher costmap_pub =
  //     nh.advertise<costmap_2d::Costmap2D>("costmap", 1000);

  try {
    std::string attrib_name = "plugin";
    std::vector<std::string> plugin_xml_paths;
    std::string xml_path =
        "/home/parallels/catkin_ws/src/simple_layer/costmap_plugins.xml";
    plugin_xml_paths.push_back(xml_path);

    pluginlib::ClassLoader<simple_layer_namespace::GridLayer> grid_layer_loader(
        "simple_layer", "costmap_2d::Layer", attrib_name, plugin_xml_paths);

    std::cout << "Loaded simple_layer plugin" << std::endl;

    boost::shared_ptr<simple_layer_namespace::GridLayer> grid =
        grid_layer_loader.createInstance("simple_layer_namespace::GridLayer");
    //... use the grid_layer, boost::shared_ptr will automatically delete memory
    // when it goes out of scope
    grid->onInitialize();
    grid->setCostDot(*master_grid.getCostmap(), 5, 5, 128);
    std::cout << master_grid.getCostmap()->getSizeInCellsY();

  } catch (pluginlib::PluginlibException& ex) {
    // handle the class failing to load
    ROS_ERROR("The plugin failed to load for some reason. Error: %s",
              ex.what());
  }

  ros::spin();

  return 0;
}
