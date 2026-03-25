#include <cmath>
#include <limits>
#include <random>
#include <string>
#include <vector>

#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGraphicsEllipseItem>
#include <QtWidgets/QGraphicsLineItem>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsTextItem>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTableWidgetItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QRegularExpression>
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QPen>

#include "run.h"

namespace tasks {
    enum class DistType {
        Uniform = 0,
        Linear = 1,
        Custom = 2,
    };

    struct LevelParams {
        double p_stop = 0.0;
        std::vector<double> child_probs;
    };

    struct SimulationResult {
        std::vector<int> path;
        int stop_level = 0;

        bool reached_leaf(int depth) const {
            return stop_level == depth;
        }

        int visited_vertices() const {
            return stop_level + 1;
        }
    };

    class SimulationDialog final : public QDialog {
    public:
        SimulationDialog() : rng_(std::random_device{}()) {
            setWindowTitle("Моделирование движения точки в M-арном дереве");
            setMinimumSize(1100, 700);

            m_spin_ = new QSpinBox();
            m_spin_->setRange(2, 10);
            m_spin_->setValue(2);

            depth_spin_ = new QSpinBox();
            depth_spin_->setRange(1, 8);
            depth_spin_->setValue(4);

            trials_spin_ = new QSpinBox();
            trials_spin_->setRange(1, 1000000);
            trials_spin_->setValue(10000);

            l_spin_ = new QSpinBox();
            l_spin_->setRange(1, depth_spin_->value() + 1);
            l_spin_->setValue(3);

            leaf_edit_ = new QLineEdit();
            leaf_edit_->setPlaceholderText("Напр.: 0 1 0 1");

            dist_combo_ = new QComboBox();
            dist_combo_->addItem("Равномерное");
            dist_combo_->addItem("Линейное (вес i+1)");
            dist_combo_->addItem("Пользовательское");

            level_table_ = new QTableWidget();
            level_table_->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
            level_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            level_table_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

            // normalize_button_ = new QPushButton("Нормализовать строки");
            simulate_button_ = new QPushButton("Смоделировать");
            sample_button_ = new QPushButton("Сгенерировать путь");

            results_box_ = new QPlainTextEdit();
            results_box_->setReadOnly(true);
            results_box_->setMinimumHeight(140);

            path_label_ = new QLabel("Путь: —");
            path_label_->setWordWrap(true);

            tree_view_ = new QGraphicsView();
            tree_view_->setRenderHint(QPainter::Antialiasing);

            status_label_ = new QLabel();
            status_label_->setWordWrap(true);

            auto *input_form = new QFormLayout();
            input_form->addRow("M (ветвистость):", m_spin_);
            input_form->addRow("Глубина (D):", depth_spin_);
            input_form->addRow("Число испытаний:", trials_spin_);
            input_form->addRow("L (число вершин):", l_spin_);
            input_form->addRow("Путь к листу:", leaf_edit_);
            input_form->addRow("Распределение:", dist_combo_);

            auto *input_group = new QGroupBox("Параметры");
            input_group->setLayout(input_form);

            auto *table_group = new QGroupBox("Параметры уровней (p_stop и вероятности переходов)");
            auto *table_layout = new QVBoxLayout();
            table_layout->addWidget(level_table_);
            // table_layout->addWidget(normalize_button_);
            table_group->setLayout(table_layout);

            auto *button_row = new QHBoxLayout();
            button_row->addWidget(simulate_button_);
            button_row->addWidget(sample_button_);

            auto *left_layout = new QVBoxLayout();
            left_layout->addWidget(input_group);
            left_layout->addWidget(table_group);
            left_layout->addLayout(button_row);
            left_layout->addWidget(new QLabel("Результаты:"));
            left_layout->addWidget(results_box_);
            left_layout->addWidget(status_label_);

            auto *left_widget = new QWidget();
            left_widget->setLayout(left_layout);

            auto *right_layout = new QVBoxLayout();
            right_layout->addWidget(tree_view_);
            right_layout->addWidget(path_label_);

            auto *right_widget = new QWidget();
            right_widget->setLayout(right_layout);

            auto *splitter = new QSplitter();
            splitter->addWidget(left_widget);
            splitter->addWidget(right_widget);
            splitter->setStretchFactor(0, 0);
            splitter->setStretchFactor(1, 1);

            auto *main_layout = new QVBoxLayout();
            main_layout->addWidget(splitter);
            setLayout(main_layout);

            rebuild_table();

            connect(m_spin_, qOverload<int>(&QSpinBox::valueChanged), this, [this](int) {
                rebuild_table();
            });
            connect(depth_spin_, qOverload<int>(&QSpinBox::valueChanged), this, [this](int) {
                rebuild_table();
            });
            connect(dist_combo_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) {
                apply_distribution_to_table();
            });
            connect(simulate_button_, &QPushButton::clicked, this, [this] {
                run_simulation();
            });
            connect(sample_button_, &QPushButton::clicked, this, [this] {
                run_sample();
            });
            /*connect(normalize_button_, &QPushButton::clicked, this, [this] {
                normalize_table();
            });*/

            run_sample();
        }

    private:
        QSpinBox *m_spin_ = nullptr;
        QSpinBox *depth_spin_ = nullptr;
        QSpinBox *trials_spin_ = nullptr;
        QSpinBox *l_spin_ = nullptr;
        QLineEdit *leaf_edit_ = nullptr;
        QComboBox *dist_combo_ = nullptr;
        QTableWidget *level_table_ = nullptr;
        QPushButton *normalize_button_ = nullptr;
        QPushButton *simulate_button_ = nullptr;
        QPushButton *sample_button_ = nullptr;
        QPlainTextEdit *results_box_ = nullptr;
        QLabel *path_label_ = nullptr;
        QGraphicsView *tree_view_ = nullptr;
        QLabel *status_label_ = nullptr;

        std::mt19937 rng_;

        DistType current_dist() const {
            return static_cast<DistType>(dist_combo_->currentIndex());
        }

        void rebuild_table() {
            int depth = depth_spin_->value();
            int m = m_spin_->value();

            l_spin_->setMaximum(depth + 1);
            if (l_spin_->value() > depth + 1) {
                l_spin_->setValue(depth + 1);
            }

            QStringList headers;
            headers << "p_stop";
            for (int i = 0; i < m; ++i) {
                headers << QString("p%1").arg(i);
            }

            level_table_->setRowCount(depth);
            level_table_->setColumnCount(m + 1);
            level_table_->setHorizontalHeaderLabels(headers);

            for (int row = 0; row < depth; ++row) {
                level_table_->setVerticalHeaderItem(row, new QTableWidgetItem(QString("L%1").arg(row)));
                set_cell_value(row, 0, 0.0, false);
            }

            apply_distribution_to_table();
        }

        void set_cell_value(int row, int col, double value, bool read_only) {
            QString text = QString::number(value, 'f', 6);
            QTableWidgetItem *item = level_table_->item(row, col);
            if (!item) {
                item = new QTableWidgetItem();
                level_table_->setItem(row, col, item);
            }
            item->setText(text);

            Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
            if (!read_only) {
                flags |= Qt::ItemIsEditable;
            }
            item->setFlags(flags);
        }

        void apply_distribution_to_table() {
            int depth = depth_spin_->value();
            int m = m_spin_->value();
            DistType dist = current_dist();

            std::vector<double> probs(m, 0.0);
            if (dist == DistType::Uniform) {
                double p = 1.0 / static_cast<double>(m);
                for (int i = 0; i < m; ++i) {
                    probs[i] = p;
                }
            } else if (dist == DistType::Linear) {
                double sum = static_cast<double>(m) * (static_cast<double>(m) + 1.0) / 2.0;
                for (int i = 0; i < m; ++i) {
                    probs[i] = (static_cast<double>(i) + 1.0) / sum;
                }
            }

            for (int row = 0; row < depth; ++row) {
                set_cell_value(row, 0, read_cell_value(row, 0), false);
                for (int col = 0; col < m; ++col) {
                    bool read_only = dist != DistType::Custom;
                    if (dist == DistType::Custom) {
                        QTableWidgetItem *item = level_table_->item(row, col + 1);
                        if (!item) {
                            set_cell_value(row, col + 1, 1.0 / static_cast<double>(m), false);
                        } else {
                            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
                        }
                    } else {
                        set_cell_value(row, col + 1, probs[col], read_only);
                    }
                }
            }
        }

        double read_cell_value(int row, int col) const {
            QTableWidgetItem *item = level_table_->item(row, col);
            if (!item) {
                return 0.0;
            }
            bool ok = false;
            double value = item->text().toDouble(&ok);
            return ok ? value : 0.0;
        }

        bool parse_leaf_path(std::vector<int> &path, QString &error) const {
            path.clear();
            const int depth = depth_spin_->value();
            const int m = m_spin_->value();
            QString text = leaf_edit_->text().trimmed();

            if (text.isEmpty()) {
                error = "Путь к листу не задан.";
                return false;
            }

            QStringList tokens = text.split(QRegularExpression("[,;\\s]+"), Qt::SkipEmptyParts);
            if (tokens.size() == 1 && tokens[0].size() == depth && m <= 10) {
                tokens.clear();
                for (const QChar &ch : text) {
                    if (ch.isSpace()) {
                        continue;
                    }
                    tokens << QString(ch);
                }
            }

            if (tokens.size() != depth) {
                error = QString("Путь должен содержать %1 чисел.").arg(depth);
                return false;
            }

            for (const QString &token : tokens) {
                bool ok = false;
                int value = token.toInt(&ok);
                if (!ok || value < 0 || value >= m) {
                    error = QString("Каждый шаг должен быть в диапазоне 0..%1.").arg(m - 1);
                    return false;
                }
                path.push_back(value);
            }

            return true;
        }

        bool read_level_params(std::vector<LevelParams> &levels, QString &error, bool &normalized) const {
            normalized = false;
            const int depth = depth_spin_->value();
            const int m = m_spin_->value();
            DistType dist = current_dist();

            levels.clear();
            levels.resize(depth);

            for (int row = 0; row < depth; ++row) {
                QTableWidgetItem *p_item = level_table_->item(row, 0);
                if (!p_item) {
                    error = QString("Не задан p_stop на уровне %1.").arg(row);
                    return false;
                }
                bool ok = false;
                double p_stop = p_item->text().toDouble(&ok);
                if (!ok || p_stop < 0.0 || p_stop > 1.0) {
                    error = QString("Неверное значение p_stop на уровне %1.").arg(row);
                    return false;
                }

                std::vector<double> probs(m, 0.0);
                if (dist == DistType::Uniform) {
                    double p = 1.0 / static_cast<double>(m);
                    for (int i = 0; i < m; ++i) {
                        probs[i] = p;
                    }
                } else if (dist == DistType::Linear) {
                    double sum = static_cast<double>(m) * (static_cast<double>(m) + 1.0) / 2.0;
                    for (int i = 0; i < m; ++i) {
                        probs[i] = (static_cast<double>(i) + 1.0) / sum;
                    }
                } else {
                    double sum = 0.0;
                    for (int col = 0; col < m; ++col) {
                        QTableWidgetItem *item = level_table_->item(row, col + 1);
                        if (!item) {
                            error = QString("Не задана вероятность перехода на уровне %1.").arg(row);
                            return false;
                        }
                        bool ok_child = false;
                        double value = item->text().toDouble(&ok_child);
                        if (!ok_child || value < 0.0) {
                            error = QString("Неверное значение вероятности на уровне %1.").arg(row);
                            return false;
                        }
                        probs[col] = value;
                        sum += value;
                    }
                    if (sum <= 0.0) {
                        error = QString("Сумма вероятностей переходов на уровне %1 должна быть > 0.").arg(row);
                        return false;
                    }
                    if (std::abs(sum - 1.0) > 1e-6) {
                        normalized = true;
                        for (double &v : probs) {
                            v /= sum;
                        }
                    }
                }

                levels[row] = LevelParams{p_stop, probs};
            }

            return true;
        }

        int sample_child(const std::vector<double> &probs, double u) {
            double cumulative = 0.0;
            for (int i = 0; i < static_cast<int>(probs.size()); ++i) {
                cumulative += probs[i];
                if (u <= cumulative) {
                    return i;
                }
            }
            return static_cast<int>(probs.size()) - 1;
        }

        SimulationResult simulate_once(const std::vector<LevelParams> &levels, int m, int depth) {
            std::uniform_real_distribution<double> dist01(0.0, 1.0);
            SimulationResult result;
            result.path.clear();

            for (int level = 0; level < depth; ++level) {
                double u = dist01(rng_);
                if (u < levels[level].p_stop) {
                    result.stop_level = level;
                    return result;
                }

                double choice = dist01(rng_);
                int child = sample_child(levels[level].child_probs, choice);
                result.path.push_back(child);
            }

            result.stop_level = depth;
            return result;
        }

        void normalize_table() {
            int depth = depth_spin_->value();
            int m = m_spin_->value();

            for (int row = 0; row < depth; ++row) {
                double sum = 0.0;
                for (int col = 0; col < m; ++col) {
                    QTableWidgetItem *item = level_table_->item(row, col + 1);
                    if (!item) {
                        continue;
                    }
                    bool ok = false;
                    double value = item->text().toDouble(&ok);
                    if (!ok || value < 0.0) {
                        continue;
                    }
                    sum += value;
                }
                if (sum <= 0.0) {
                    continue;
                }
                for (int col = 0; col < m; ++col) {
                    QTableWidgetItem *item = level_table_->item(row, col + 1);
                    if (!item) {
                        continue;
                    }
                    bool ok = false;
                    double value = item->text().toDouble(&ok);
                    if (!ok || value < 0.0) {
                        value = 0.0;
                    }
                    set_cell_value(row, col + 1, value / sum, false);
                }
            }
        }

        void run_simulation() {
            status_label_->setText("");

            std::vector<int> leaf_path;
            QString error;
            if (!parse_leaf_path(leaf_path, error)) {
                QMessageBox::warning(this, "Ошибка", error);
                return;
            }

            std::vector<LevelParams> levels;
            bool normalized = false;
            if (!read_level_params(levels, error, normalized)) {
                QMessageBox::warning(this, "Ошибка", error);
                return;
            }

            const int m = m_spin_->value();
            const int depth = depth_spin_->value();
            const int trials = trials_spin_->value();
            const int target_l = l_spin_->value();

            long long count_leaf = 0;
            long long count_l = 0;

            SimulationResult sample;
            for (int i = 0; i < trials; ++i) {
                SimulationResult result = simulate_once(levels, m, depth);
                sample = result;

                if (result.reached_leaf(depth)) {
                    if (result.path.size() == leaf_path.size()) {
                        bool match = true;
                        for (size_t k = 0; k < leaf_path.size(); ++k) {
                            if (result.path[k] != leaf_path[k]) {
                                match = false;
                                break;
                            }
                        }
                        if (match) {
                            ++count_leaf;
                        }
                    }
                }

                if (result.visited_vertices() == target_l) {
                    ++count_l;
                }
            }

            double p_leaf = static_cast<double>(count_leaf) / static_cast<double>(trials);
            double p_l = static_cast<double>(count_l) / static_cast<double>(trials);

            QString result_text;
            result_text += QString("Испытаний: %1\n").arg(trials);
            result_text += QString("P(попадания в заданный лист) = %1\n")
                               .arg(p_leaf, 0, 'f', 6);
            result_text += QString("P(пройдет L вершин) = %1\n")
                               .arg(p_l, 0, 'f', 6);
            result_text += QString("L = %1 (число посещенных вершин, включая корень)").arg(target_l);

            results_box_->setPlainText(result_text);
            if (normalized) {
                status_label_->setText("Внимание: вероятности переходов были нормализованы.");
            }

            update_path_label(sample, depth);
            draw_tree(sample, m, depth);
        }

        void run_sample() {
            std::vector<LevelParams> levels;
            QString error;
            bool normalized = false;
            if (!read_level_params(levels, error, normalized)) {
                status_label_->setText(error);
                return;
            }

            SimulationResult sample = simulate_once(levels, m_spin_->value(), depth_spin_->value());
            update_path_label(sample, depth_spin_->value());
            draw_tree(sample, m_spin_->value(), depth_spin_->value());
        }

        void update_path_label(const SimulationResult &sample, int depth) {
            QStringList parts;
            for (int step : sample.path) {
                parts << QString::number(step);
            }
            QString path_text = parts.isEmpty() ? "корень" : parts.join(" -> ");
            QString stop_text;
            if (sample.reached_leaf(depth)) {
                stop_text = QString("лист (уровень %1)").arg(depth);
            } else {
                stop_text = QString("внутренний узел (уровень %1)").arg(sample.stop_level);
            }
            path_label_->setText(QString("Путь: %1. Остановка: %2. Посещено вершин: %3.")
                                     .arg(path_text)
                                     .arg(stop_text)
                                     .arg(sample.visited_vertices()));
        }

        void draw_tree(const SimulationResult &sample, int m, int depth) {
            auto *scene = new QGraphicsScene(tree_view_);
            tree_view_->setScene(scene);
            scene->setBackgroundBrush(QBrush(QColor(250, 250, 250)));

            const int max_nodes = 2000;
            bool full = true;

            long long nodes_on_level = 1;
            long long total_nodes = 1;
            for (int level = 1; level <= depth; ++level) {
                if (nodes_on_level > max_nodes / m) {
                    full = false;
                    break;
                }
                nodes_on_level *= m;
                if (total_nodes > max_nodes - nodes_on_level) {
                    full = false;
                    break;
                }
                total_nodes += nodes_on_level;
            }
            if (nodes_on_level > 512) {
                full = false;
            }

            double width_units = 1.0;
            for (int i = 0; i < depth; ++i) {
                width_units *= static_cast<double>(m);
            }
            if (width_units < 1.0) {
                width_units = 1.0;
            }

            const double x_spacing = 40.0;
            const double y_spacing = 80.0;

            if (full) {
                std::vector<std::vector<QPointF>> pos(depth + 1);
                long long count_on_level = 1;
                for (int level = 0; level <= depth; ++level) {
                    pos[level].resize(static_cast<size_t>(count_on_level));
                    double step = width_units / static_cast<double>(count_on_level);
                    for (long long i = 0; i < count_on_level; ++i) {
                        double x = (static_cast<double>(i) + 0.5) * step * x_spacing;
                        double y = static_cast<double>(level) * y_spacing;
                        pos[level][static_cast<size_t>(i)] = QPointF(x, y);
                    }
                    if (level < depth) {
                        count_on_level *= m;
                    }
                }

                QPen edge_pen(QColor(190, 190, 190));
                for (int level = 0; level < depth; ++level) {
                    long long parent_count = static_cast<long long>(pos[level].size());
                    for (long long i = 0; i < parent_count; ++i) {
                        for (int child = 0; child < m; ++child) {
                            long long child_index = i * m + child;
                            scene->addLine(QLineF(pos[level][static_cast<size_t>(i)],
                                                  pos[level + 1][static_cast<size_t>(child_index)]),
                                           edge_pen);
                        }
                    }
                }

                QBrush node_brush(QColor(245, 245, 245));
                QPen node_pen(QColor(120, 120, 120));
                const double radius = 6.0;
                for (int level = 0; level <= depth; ++level) {
                    for (const QPointF &p : pos[level]) {
                        scene->addEllipse(p.x() - radius, p.y() - radius, radius * 2.0, radius * 2.0,
                                          node_pen, node_brush);
                    }
                }

                highlight_path(sample, m, depth, pos, scene);
            } else {
                QGraphicsTextItem *note = scene->addText("Дерево слишком большое, показан только путь.");
                note->setDefaultTextColor(QColor(120, 120, 120));
                note->setPos(0, 0);

                std::vector<QPointF> path_points;
                int node_index = 0;
                for (int level = 0; level <= sample.stop_level; ++level) {
                    double nodes = std::pow(static_cast<double>(m), level);
                    double step = width_units / nodes;
                    double x = (static_cast<double>(node_index) + 0.5) * step * x_spacing;
                    double y = static_cast<double>(level) * y_spacing;
                    path_points.push_back(QPointF(x, y));
                    if (level < static_cast<int>(sample.path.size())) {
                        node_index = node_index * m + sample.path[static_cast<size_t>(level)];
                    }
                }

                QPen path_pen(QColor(200, 40, 40), 3.0);
                QBrush path_brush(QColor(200, 40, 40));
                const double radius = 6.0;
                for (size_t i = 0; i < path_points.size(); ++i) {
                    if (i + 1 < path_points.size()) {
                        scene->addLine(QLineF(path_points[i], path_points[i + 1]), path_pen);
                    }
                    scene->addEllipse(path_points[i].x() - radius, path_points[i].y() - radius,
                                      radius * 2.0, radius * 2.0, path_pen, path_brush);
                }
            }

            tree_view_->fitInView(scene->itemsBoundingRect().adjusted(-20, -20, 20, 20),
                                  Qt::KeepAspectRatio);
        }

        void highlight_path(const SimulationResult &sample,
                            int m,
                            int depth,
                            const std::vector<std::vector<QPointF>> &pos,
                            QGraphicsScene *scene) {
            std::vector<int> node_indices;
            node_indices.reserve(sample.path.size() + 1);

            int current = 0;
            node_indices.push_back(current);

            for (int step : sample.path) {
                current = current * m + step;
                node_indices.push_back(current);
            }

            QPen path_pen(QColor(200, 40, 40), 3.0);
            QBrush path_brush(QColor(200, 40, 40));
            const double radius = 6.5;

            for (size_t level = 0; level + 1 < node_indices.size(); ++level) {
                int parent_index = node_indices[level];
                int child_index = node_indices[level + 1];
                scene->addLine(QLineF(pos[level][static_cast<size_t>(parent_index)],
                                      pos[level + 1][static_cast<size_t>(child_index)]),
                               path_pen);
            }

            for (size_t level = 0; level < node_indices.size(); ++level) {
                int node_index = node_indices[level];
                QPointF p = pos[level][static_cast<size_t>(node_index)];
                scene->addEllipse(p.x() - radius, p.y() - radius, radius * 2.0, radius * 2.0,
                                  path_pen, path_brush);
            }

            if (!sample.reached_leaf(depth)) {
                int stop_level = sample.stop_level;
                int node_index = node_indices[static_cast<size_t>(stop_level)];
                QPointF p = pos[static_cast<size_t>(stop_level)][static_cast<size_t>(node_index)];
                QPen stop_pen(QColor(30, 30, 30), 2.0);
                scene->addLine(p.x() - 8.0, p.y() - 8.0, p.x() + 8.0, p.y() + 8.0, stop_pen);
                scene->addLine(p.x() - 8.0, p.y() + 8.0, p.x() + 8.0, p.y() - 8.0, stop_pen);
            }
        }
    };

    void task2::run(int argc, char **argv) {
        QApplication *existing = qobject_cast<QApplication *>(QApplication::instance());
        if (existing) {
            SimulationDialog dialog;
            dialog.exec();
            return;
        }

        QApplication app(argc, argv);
        SimulationDialog dialog;
        dialog.exec();
    }
}
