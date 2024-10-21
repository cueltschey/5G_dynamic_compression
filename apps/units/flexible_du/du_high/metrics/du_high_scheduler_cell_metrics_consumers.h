/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "apps/services/metrics/metrics_consumer.h"
#include "du_high_scheduler_cell_metrics.h"
#include "srsran/srslog/log_channel.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// Consumer for the STDOUT scheduler cell metrics.
class scheduler_cell_metrics_consumer_stdout : public app_services::metrics_consumer
{
public:
  explicit scheduler_cell_metrics_consumer_stdout(bool print_metrics_) : print_metrics(print_metrics_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

  /// This can be called from another execution context to turn on/off the actual plotting.
  void toggle_print() { print_metrics = !print_metrics; }

private:
  unsigned          nof_lines     = 10;
  std::atomic<bool> print_metrics = {false};
};

/// Consumer for the json scheduler cell metrics.
class scheduler_cell_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit scheduler_cell_metrics_consumer_json(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel& log_chan;
};

/// Consumer for the log file scheduler cell metrics.
class scheduler_cell_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit scheduler_cell_metrics_consumer_log(srslog::basic_logger& logger_) : logger(logger_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::basic_logger& logger;
};

/// Consumer for the E2 file scheduler cell metrics.
class scheduler_cell_metrics_consumer_e2 : public app_services::metrics_consumer
{
public:
  explicit scheduler_cell_metrics_consumer_e2(scheduler_metrics_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  scheduler_metrics_notifier& notifier;
};

} // namespace srsran