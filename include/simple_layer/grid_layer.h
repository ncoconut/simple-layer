#ifndef GRID_LAYER_H_
#define GRID_LAYER_H_
#include <costmap_2d/GenericPluginConfig.h>
#include <costmap_2d/layer.h>
#include <costmap_2d/layered_costmap.h>
#include <dynamic_reconfigure/server.h>
#include <ros/ros.h>

// Two key differences of this class
// This layer now extends the Costmap2D class
// isDiscretized is set to True (used for resizing)

namespace simple_layer_namespace {
class GridLayer : public costmap_2d::Layer, public costmap_2d::Costmap2D {
 public:
  GridLayer();

  virtual void onInitialize();
  virtual void updateBounds(double robot_x, double robot_y, double robot_yaw,
                            double* min_x, double* min_y, double* max_x,
                            double* max_y);
  virtual void updateCosts(costmap_2d::Costmap2D& master_grid, int min_i,
                           int min_j, int max_i, int max_j);
  bool isDiscretized() { return true; }

  virtual void matchSize();
  void setCostDot(costmap_2d::Costmap2D& master_grid, int mx, int my, int cost);

 private:
  void reconfigureCB(costmap_2d::GenericPluginConfig& config, uint32_t level);
  dynamic_reconfigure::Server<costmap_2d::GenericPluginConfig>* dsrv_;
};
}
#endif
