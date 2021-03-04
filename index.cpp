#include <vector>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <algorithm>

float EDGE_MAX_COUNT = 0.0;

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

    EDGE_MAX_COUNT += 1.0;
  }
}

void start_bron_kerbosch_without_pivot(std::set<int>& R, std::set<int>& P, std::set<int>& X, std::map< int, std::set<int> >& graph) {
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
    start_bron_kerbosch_without_pivot(
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

void bron_kerbosch_without_pivot(std::map< int, std::set<int> >& graph) {
  std::set<int> R;
  std::set<int> P;
  std::set<int> X;

  for(std::map< int, std::set<int> >::iterator iter = graph.begin(); iter != graph.end(); ++iter){
    P.insert(iter->first);
  }

  start_bron_kerbosch_without_pivot(R, P, X, graph);
}

int find_pivot(std::set<int>& P_X_union, std::map< int, std::set<int> >& graph) {
  // find the one with more neighbors
  int biggest_one_count = 0;
  int biggest_one_number;

  for (std::set<int>::iterator iter = P_X_union.begin(); iter != P_X_union.end(); iter++) {
    int neighbors_size = graph[*iter].size();

    if(biggest_one_count < neighbors_size) {
      biggest_one_count = neighbors_size;
      biggest_one_number = *iter;
    }
  }

  return biggest_one_number;
}

void start_bron_kerbosch_with_pivot(std::set<int>& R, std::set<int>& P, std::set<int>& X, std::map< int, std::set<int> >& graph) {
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

  // choose a pivot vertex u in P ⋃ X
  std::set<int> P_X_union;
  set_union(
    P.begin(), P.end(), X.begin(), X.end(),
    std::inserter(P_X_union, P_X_union.begin())
  );


  // P \ N(u)
  std::set<int> pivot_neighbors = graph[find_pivot(P_X_union, graph)];
  std::set<int> P_to_iterate = P;
  set_difference(
    P.begin(), P.end(), pivot_neighbors.begin(), pivot_neighbors.end(),
    std::inserter(P_to_iterate, P_to_iterate.begin())
  );

  //  for each vertex v in P \ N(u) do
  for (std::set<int>::iterator iter = P_to_iterate.begin(); iter != P_to_iterate.end(); iter++) {
    std::set<int> vertex_set;
    vertex_set.insert(*iter);

    // R ⋃ {v}
    std::set<int> R_v_union;
    set_union(
      R.begin(), R.end(), vertex_set.begin(), vertex_set.end(),
      std::inserter(R_v_union, R_v_union.begin())
    );

    std::set<int> neighbors = graph[*iter];

    // P ⋂ N(v)
    std::set<int> P_neighbors_intersect;
    set_intersection(
      P.begin(), P.end(), neighbors.begin(), neighbors.end(),
      std::inserter(P_neighbors_intersect, P_neighbors_intersect.begin())
    );

    // X ⋂ N(v))
    std::set<int> X_neighbors_intersect;
    set_intersection(
      X.begin(), X.end(), neighbors.begin(), neighbors.end(),
      std::inserter(X_neighbors_intersect, X_neighbors_intersect.begin())
    );

    // BronKerbosch1(R ⋃ {v}, P ⋂ N(v), X ⋂ N(v))
    start_bron_kerbosch_with_pivot(
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

void bron_kerbosch_with_pivot(std::map< int, std::set<int> >& graph) {
  std::set<int> R;
  std::set<int> P;
  std::set<int> X;

  for(std::map< int, std::set<int> >::iterator iter = graph.begin(); iter != graph.end(); ++iter){
    P.insert(iter->first);
  }

  start_bron_kerbosch_with_pivot(R, P, X, graph);
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

float find_clustering_coefficient(std::map< int, std::set<int> >& graph) {
  int node_count = graph.size();

  float clustering_coefficient[node_count];

  for (std::map< int, std::set<int> >::iterator iter = graph.begin(); iter != graph.end(); ++iter) {
    float neighbors_size = (iter->second).size();

    if(neighbors_size == 1 || neighbors_size == 0) {
      clustering_coefficient[iter->first] = 0.0;
    }

    if(2 <= neighbors_size) {
      clustering_coefficient[iter->first] = neighbors_size / EDGE_MAX_COUNT;
    }
  }

  float clustering_coefficient_sum = 0.0;
  for (std::map< int, std::set<int> >::iterator iter = graph.begin(); iter != graph.end(); ++iter) {
    clustering_coefficient_sum = clustering_coefficient_sum + clustering_coefficient[iter->first];
  }

  return (clustering_coefficient_sum / node_count);
}

int main(int argc, char const *argv[]) {
  std::map< int, std::set<int> > graph;

  int state = 0;
  populate_graph(graph, argv[1]);

  while(state != 5) {
    printf("\n");
    printf("Escolha uma ação:\n");
    printf("1 - Mostrar lista de adjacencia\n");
    printf("2 - BronKerbosch sem pivot:\n");
    printf("3 - BronKerbosch com pivot:\n");
    printf("4 - Coeficiente de aglomeração:\n");
    printf("5 - Sair:\n");
    scanf("%i", &state);

    switch (state) {
      case 1: print_graph_list(graph);
      case 2: bron_kerbosch_without_pivot(graph);
      case 3: bron_kerbosch_with_pivot(graph);
      case 4: printf("Coeficiente de aglomeração: %f\n", find_clustering_coefficient(graph));
      case 5: break;
    }
  }

  return 0;
}
