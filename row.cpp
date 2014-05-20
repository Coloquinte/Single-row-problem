#include <iostream>
#include <vector>
#include <queue>
#include <cassert>
#include <stdexcept>
#include <algorithm>

class fixed_order_row{
	struct bound{
		int absolute_position; // Position of the left side of the first cell if they are all clustered when the bound is active
		int slope_change;

		bool operator<(bound const a) const{return absolute_position < a.absolute_position;}

        bound(int pos, int sl) : absolute_position(pos), slope_change(sl){}
        bound(){}
	};

	std::priority_queue<bound> bounds;
    std::vector<int> widths;
    std::vector<int> place_time_positions; // Position of the left side of the first cell if they are all clustered

	int current_width;
	int begin, end;


	public:
    size_t size() const{
		assert(widths.size() == place_time_positions.size());
		return widths.size();
	}

	fixed_order_row(int b, int e){
		begin = b;
		end   = e;
		current_width = 0;
	}

    void simple_push(std::vector<int> cell_bounds_pos, std::vector<int> cell_slope_changes, int cell_right_slope, int cell_width){
        if(cell_bounds_pos.size() != cell_slope_changes.size()){
            throw std::runtime_error("Mismatched vector sizes");
        }
		if(begin + current_width > end){
			throw std::runtime_error("Row capacity exceded");
		}
       
		int new_width = current_width + cell_width;
        int current_slope = cell_right_slope;

        for(int i=0; i<cell_bounds_pos.size(); ++i){
            if(cell_bounds_pos[i] > begin + current_width){
                bounds.push( bound(cell_bounds_pos[i] - current_width, cell_slope_changes[i]) );
            }
        }
 
        int last_admissible_position = end - new_width;

        // Remove the out-of-range bounds
        while(not bounds.empty() && bounds.top().absolute_position > last_admissible_position){
            current_slope -= bounds.top().slope_change;
            bounds.pop();
        }

        int last_bound = last_admissible_position;

        // Remove the bounds in order until we reach the non-equilibrium point
        while(not bounds.empty() && current_slope > 0){
            current_slope -= bounds.top().slope_change;
            last_bound = bounds.top().absolute_position;
            bounds.pop();
        }

        if(current_slope > 0){ // No bound left, left packed placement
            last_bound = begin;
        }
        else if(current_slope < 0){ // The last bound wasn't entirely consumed or the placement is right-packed
            bounds.push( bound(last_bound, -current_slope) );
        }

        current_width = new_width;
        place_time_positions.push_back(last_bound);
        widths.push_back(cell_width);
    }

	std::vector<int> get_positions() const{
		std::vector<int> ret(size());
		
		// We have the positions where each element was placed
		// We calculate how other elements where pushed
		
        int constraint = end;
        int tot_width = current_width;
        for(int i=size()-1; i>=0; --i){
            constraint -= widths[i];
            tot_width  -= widths[i];
            constraint = std::min(place_time_positions[i] + tot_width, constraint); // The specified position, or the position constrained by the previous placement
            ret[i] = constraint;
        }
		return ret;
    }

};


int main(){

    int begin, end;
    
    std::cout << "Enter the begin and the end of the row: ";
    std::cin >> begin >> end;

    fixed_order_row row(begin, end);

    while(true){
        int nbr_bnds, right_slope, width;
        std::cout << std::endl << "Enter the number of bounds of the cell (-1 to quit): ";
        std::cin >> nbr_bnds;
        if(nbr_bnds < 0){
            break;
        }

        std::cout << std::endl << "Enter its width: ";
        std::cin >> width;
        std::cout << std::endl << "Enter its slope on the right (positive means it is pulled to the left): ";
        std::cin >> right_slope;

        std::vector<int> positions, slope_changes;
        for(int i=0; i<nbr_bnds; ++i){
            int pos, slope_change;
            std::cout << std::endl << "Position and slope change: ";
            std::cin >> pos >>  slope_change;
            positions.push_back(pos);
            slope_changes.push_back(slope_change);
        }
        row.simple_push(positions, slope_changes, right_slope, width);
    }

    std::cout << std::endl << "The row has " << row.size() << " cells." << std::endl;
    std::cout << "Their positions are:";
    for(int pos : row.get_positions()){
        std::cout << " " << pos;
    }
    std::cout << std::endl;

}
