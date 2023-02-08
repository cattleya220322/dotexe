#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <algorithm> // shuffle()
#include <cstdlib>   // getenv()

namespace Daily
{
  /// @brief 区切り文字を含む文字列を分割してコンテナ化する
  /// @param target_str 区切り文字を含む文字列
  /// @param delimiter 区切り文字
  /// @return 分割後のコンテナ
  auto Split(std::string const &target_str, char delimiter) -> std::vector<std::string>
  {
    // 読み取る対象として文字列ストリームを定義
    std::istringstream target_sstream(target_str);
    // 分割後の文字列要素
    std::string split_str;
    // 分割後の文字列コンテナ
    std::vector<std::string> vector_str;

    while (std::getline(target_sstream, split_str, delimiter))
      vector_str.push_back(split_str);

    return vector_str;
  }

  /// @brief ランダムな整数（0~(length-1)）を生成
  /// @param length 例えばコンテナのサイズ
  /// @return 例えばコンテナのランダムなインデックス
  auto GenRandom(int length) -> int
  {
    // 擬似乱数生成エンジンのシード初期化を行う、非決定論的な乱数生成エンジン
    std::random_device gen_seed;
    // std::random_device::operator() 非決定論的な乱数の生成を行う
    // mersenne_twister_engineのパラメータ設定済み別名であるmt19937を使用する。
    // std::mersenne_twister_engine::operator() 擬似乱数の生成を行う
    // シード値を指定して再初期化
    std::mt19937 engine(gen_seed());

    // 分布の最小最大値を決定
    int const kLowerBound = 0;
    int const kUpperBound = length - 1;

    // 抽選結果の数字を等確率でランダムに生成する分布
    std::uniform_int_distribution<> dist(kLowerBound, kUpperBound);

    return dist(engine);
  }

  /// @brief vector の要素順をシャッフル
  /// @tparam T 要素のデータ型
  /// @param list シャッフルする vector
  template <class T>
  void ShuffleList(std::vector<T> &list)
  {
    std::random_device gen_seed;
    std::mt19937 engine(gen_seed());
    std::shuffle(list.begin(), list.end(), engine);
  }

  /// @brief 文字列の部分置換（初めに見つかったモノのみ）
  /// @param str_src 検索・置換対象の文字列
  /// @param str_old 検索文字列
  /// @param str_new 置換文字列
  void Replace(std::string &str_src, std::string const &str_old, std::string const &str_new)
  {
    auto pos = str_src.find(str_old);
    str_src.replace(pos, str_old.length(), str_new);
  }
}

/// @brief csv ファイル中のリストからランダムなデイリーミッションを表示する
/// @return 0: 正常終了
auto main(int argc, char *argv[]) -> int
{
  std::string name_file;
  switch (argc)
  {
  case 1:
    name_file = "list_daily_mission.csv";
    break;
  case 2:
    // >> .Exe/DailyMission/daily_mission '~/.Exe/DailyMission/list_daily_mission.csv'
    {
      std::string path_arg{argv[1]};
      std::string const symbol_home{"~"};
      std::string const path_home{std::getenv("HOME")};
      Daily::Replace(path_arg, symbol_home, path_home);
      name_file = path_arg;
    }
    // >> ~/.Exe/DailyMission/daily_mission ~/.Exe/DailyMission/list_daily_mission.csv
    // name_file = argv[1];
    break;
  default:
    std::cout << "incorrect argument" << std::endl;
    return 1;
  }
  std::ifstream ifs(name_file);
  // １行（１分野）
  std::string row_str;
  // デイリーミッションリスト
  std::vector<std::vector<std::string>> list_daily;
  // 全行（全分野）に渡って
  while (std::getline(ifs, row_str))
  {
    std::vector<std::string> elems_str_in_row = Daily::Split(row_str, ',');
    // 先頭位置にカテゴリを記入しているので、これをコピー・削除
    std::string category_str = elems_str_in_row[0];
    elems_str_in_row.erase(elems_str_in_row.begin());
    // １行（１分野）の中からランダムにミッションを１つ抽出する
    std::string elem_str = elems_str_in_row.at(Daily::GenRandom(elems_str_in_row.size()));
    // カテゴリとミッションのセットを作る
    std::vector<std::string> set_category_elem = {category_str, elem_str};
    // カテゴリとミッションのセットのベクタ
    list_daily.push_back(set_category_elem);
  }
  // デイリーミッションリストの順番をシャッフル
  Daily::ShuffleList(list_daily);
  // デイリーミッションを表示
  for (int i = 0; i < static_cast<int>(list_daily.size()); i++)
    std::cout << i + 1 << "." << '[' << list_daily[i][0] << ']' << list_daily[i][1] << '\n';
  std::cout << std::flush;
}
