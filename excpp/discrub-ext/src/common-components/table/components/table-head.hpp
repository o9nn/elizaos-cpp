#include "...enum/sort-direction.hpp"
#include ".table.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;
;
;
;
;
;
;

type TableHeadProps<T> = {
  columns: TableColumn<T>[];
  selectedRows: string[];
  rowCount: number;
  order: SortDirection;
  orderBy: keyof T;
  handleRequestSort: (_: unknown, property: keyof T) => void;
  handleSelectAllClick: (event: React.ChangeEvent<HTMLInputElement>) => void;
};

const TableHead = <T,>({
  columns,
  selectedRows,
  rowCount,
  order,
  orderBy,
  handleRequestSort,
  handleSelectAllClick,
}: TableHeadProps<T>) => {
  const createSortHandler = (property: keyof T) => (_: unknown) => {
    handleRequestSort(_, property);
  };

  return (
    <TbHead>
      <TableRow>
        <TableCell padding="checkbox">
          <Checkbox
            indeterminate={
              selectedRows.length > 0 && selectedRows.length < rowCount
            }
            checked={rowCount > 0 && selectedRows.length === rowCount}
            onChange={handleSelectAllClick}
            color="secondary"
          />
        </TableCell>
        {columns.map((column) => (
          <TableCell
            key={String(column.id)}
            align="left"
            padding={column.disablePadding ? "none" : "normal"}
            sortDirection={orderBy === column.id ? order : false}
          >
            <TableSortLabel
              active={orderBy === column.id}
              direction={orderBy === column.id ? order : "asc"}
              onClick={createSortHandler(column.id)}
            >
              {column.label}
              {orderBy === column.id ? (
                <Box component="span" sx={visuallyHidden}>
                  {order === "desc" ? "sorted descending" : "sorted ascending"}
                </Box>
              ) : null}
            </TableSortLabel>
          </TableCell>
        ))}
        <TableCell padding="checkbox" />
      </TableRow>
    </TbHead>
  );
};

default TableHead;

} // namespace elizaos
