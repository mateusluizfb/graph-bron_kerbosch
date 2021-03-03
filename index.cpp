#include <vector>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <algorithm>

void populate_graph(std::map< int, std::set<int> >& graph, char const argv[]) {
  std::ifstream dolphins_data(argv);

  const std::string DATA_FILE_START_STATEMENT = "62 62 159";

  bool is_real_data = false;
  std::string line;

  while(getline(dolphins_data, line)) {
    if(!(line.compare(DATA_FILE_START_STATEMENT))){
      is_real_data = true;
      continue;
    }

    if(is_real_data == false) continue;

    std::string vertex = line.c_str();
    std::string delimiter = " ";
    int node1 = std::stoi(vertex.substr(0, 2));
    int node2 = std::stoi(vertex.substr(3, 3));

    graph[node1].insert(node2);
    graph[node2].insert(node1);
  }
}

void start_bron_kerbosch_wihthout_pivot(std::set<int>& R, std::set<int>& P, std::set<int>& X, std::map< int, std::set<int> >& graph) {
  // if P and X are both empty then
  if(P.empty() && X.empty()) {
    // report R as a maximal clique
    printf("Maximal clique: ");
    for (std::set<int>::iterator iter = R.begin(); iter != R.end(); iter++) {
      printf("%i ", *iter);
    }
    printf("\n");

    return;
  }

  //  for each vertex v in P do
  std::set<int> P_to_iterate = P;
  for (std::set<int>::iterator iter = P_to_iterate.begin(); iter != P_to_iterate.end(); iter++) {
    std::set<int> vertex_set;
    vertex_set.insert(*iter);

    std::set<int> R_v_union;
    set_union(
      R.begin(), R.end(), vertex_set.begin(), vertex_set.end(),
      std::inserter(R_v_union, R_v_union.begin())
    );

    std::set<int> neighbors = graph[*iter];
    std::set<int> P_neighbors_intersect;
    set_intersection(
      P.begin(), P.end(), neighbors.begin(), neighbors.end(),
      std::inserter(P_neighbors_intersect, P_neighbors_intersect.begin())
    );
    std::set<int> X_neighbors_intersect;
    set_intersection(
      X.begin(), X.end(), neighbors.begin(), neighbors.end(),
      std::inserter(X_neighbors_intersect, X_neighbors_intersect.begin())
    );

    // BronKerbosch1(R ⋃ {v}, P ⋂ N(v), X ⋂ N(v))
    start_bron_kerbosch_wihthout_pivot(
      R_v_union,
      P_neighbors_intersect,
      X_neighbors_intersect,
      graph
    );

    // P = P \ {v}
    std::set<int> P_v_difference;
    set_difference(
      P.begin(), P.end(), vertex_set.begin(), vertex_set.end(),
      std::inserter(P_v_difference, P_v_difference.begin())
    );
    P = P_v_difference;

    // X = X union {v}
    std::set<int> X_v_union;
    set_union(
      X.begin(), X.end(), vertex_set.begin(), vertex_set.end(),
      std::inserter(X_v_union, X_v_union.begin())
    );

    X = X_v_union;
  }
}

void bron_kerbosch_wihthout_pivot(std::map< int, std::set<int> >& graph) {
  std::set<int> R;
  std::set<int> P;
  std::set<int> X;

  for(std::map< int, std::set<int> >::iterator iter = graph.begin(); iter != graph.end(); ++iter){
    P.insert(iter->first);
  }

  start_bron_kerbosch_wihthout_pivot(R, P, X, graph);
}

void print_graph_list(std::map< int, std::set<int> >& graph) {
  for(std::map< int, std::set<int> >::iterator iter = graph.begin(); iter != graph.end(); ++iter){
    int key =  iter->first;
    printf("%i: ", key);

    for (int value : iter->second) {
      printf("%i ", value);
    }

    printf("\n");
  }
}

int main(int argc, char const *argv[]) {
  std::map< int, std::set<int> > graph;

  int state = 0;
  populate_graph(graph, argv[1]);

  while(state != 4) {
    printf("\n");
    printf("Escolha uma ação:\n");
    printf("1 - Mostrar lista de adjacencia\n");
    printf("2 - BronKerbosch sem pivot:\n");
    printf("3 - BronKerbosch com pivot:\n");
    printf("4 - Sair:\n");
    scanf("%i", &state);

    switch (state) {
      case 1: print_graph_list(graph);
      case 2: bron_kerbosch_wihthout_pivot(graph);
      case 3: bron_kerbosch_wihthout_pivot(graph);
      case 4: break;
    }
  }

  return 0;
}
