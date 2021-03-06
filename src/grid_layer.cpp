#include <pluginlib/class_list_macros.h>
#include <simple_layer/grid_layer.h>

PLUGINLIB_EXPORT_CLASS(simple_layer_namespace::GridLayer, costmap_2d::Layer)

using costmap_2d::LETHAL_OBSTACLE;
using costmap_2d::NO_INFORMATION;

namespace simple_layer_namespace {

GridLayer::GridLayer() {}

// In initialization, we set default_value_ and call matchSize. Match size
// matches the size of the master grid and fills it with the value specified in
// default_value_, which by default is zero.
void GridLayer::onInitialize() {
  ros::NodeHandle nh("~/" + name_);
  current_ = true;
  default_value_ = NO_INFORMATION;
  matchSize();

  dsrv_ = new dynamic_reconfigure::Server<costmap_2d::GenericPluginConfig>(nh);
  dynamic_reconfigure::Server<costmap_2d::GenericPluginConfig>::CallbackType
      cb = boost::bind(&GridLayer::reconfigureCB, this, _1, _2);
  dsrv_->setCallback(cb);
}

// We also implement the matchSize method to do what we said above. This is left
// to the user to implement for cases where additional adjustments need to be
// made.
void GridLayer::matchSize() {
  Costmap2D* master = layered_costmap_->getCostmap();
  resizeMap(master->getSizeInCellsX(), master->getSizeInCellsY(),
            master->getResolution(), master->getOriginX(),
            master->getOriginY());
}

void GridLayer::reconfigureCB(costmap_2d::GenericPluginConfig& config,
                              uint32_t level) {
  enabled_ = config.enabled;
}

// Update the bounds of the costmap
void GridLayer::updateBounds(double robot_x, double robot_y, double robot_yaw,
                             double* min_x, double* min_y, double* max_x,
                             double* max_y) {
  if (!enabled_) return;

  double mark_x = robot_x + cos(robot_yaw), mark_y = robot_y + sin(robot_yaw);
  unsigned int mx;
  unsigned int my;
  if (worldToMap(mark_x, mark_y, mx, my)) {
    setCost(mx, my, LETHAL_OBSTACLE);
  }

  *min_x = std::min(*min_x, mark_x);
  *min_y = std::min(*min_y, mark_y);
  *max_x = std::max(*max_x, mark_x);
  *max_y = std::max(*max_y, mark_y);
}

// Instead of setting the value in updateCosts, we set the value in the layer's
// own costmap using the setCost method.
// Now in the updateCosts method, we overwrite the previous values of the master
// costmap to include our marks. We do not overwrite if the layer's costmap
// value is NO_INFORMATION.
void GridLayer::updateCosts(costmap_2d::Costmap2D& master_grid, int min_i,
                            int min_j, int max_i, int max_j) {
  if (!enabled_) return;

  for (int j = min_j; j < max_j; j++) {
    for (int i = min_i; i < max_i; i++) {
      int index = getIndex(i, j);
      if (costmap_[index] == NO_INFORMATION) continue;
      master_grid.setCost(i, j, costmap_[index]);
    }
  }
}

void GridLayer::setCostDot(costmap_2d::Costmap2D& master_grid, int mx, int my,
                           int cost) {
  if (!enabled_) return;

  master_grid.setCost((unsigned int)mx, (unsigned int)my, (unsigned char)cost);
}

}  // end namespace
