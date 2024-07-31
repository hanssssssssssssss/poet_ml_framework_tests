scale_min_max <- function(x, min, max, backtransform) {
  if (backtransform) {
      return((x * (max - min)) + min)
  } else {
      return((x - min) / (max - min))
  }
}

##' @title Apply transformations to cols of a data.frame
##' @param df named data.frame
##' @param tlist list of function names
##' @return data.frame with the columns specified in tlist and the
##'     transformed numerical values
##' @author MDL
TransfList <- function (df, tlist) {
    vars <- intersect(colnames(df), names(tlist))
    lens <- sapply(tlist[vars], length)
    n <- max(lens)
    filledlist <- lapply(tlist[vars], function(x) if (length(x) < 
        n) 
        return(c(x, rep("I", n - length(x))))
    else return(x))
    tmp <- df[, vars]
    for (i in seq_len(n)) {
        tmp <- as.data.frame(sapply(vars, function(x) do.call(filledlist[[x]][i], 
            list(tmp[, x]))))
    }
    return(tmp)
}

##' This function applies some specified string substitution such as
##' 's/log/exp/' so that from a list of "forward" transformation
##' functions it computes a "backward" one
##' @title Apply back transformations to cols of a data.frame
##' @param df named data.frame
##' @param tlist list of function names
##' @return data.frame with the columns specified in tlist and the
##'     backtransformed numerical values
##' @author MDL
BackTransfList <- function (df, tlist) {
    vars <- intersect(colnames(df), names(tlist))
    lens <- sapply(tlist[vars], length)
    n <- max(lens)
    filledlist <- lapply(tlist[vars], function(x) if (length(x) < 
        n) 
        return(c(x, rep("I", n - length(x))))
    else return(x))
    rlist <- lapply(filledlist, rev)
    rlist <- lapply(rlist, sub, pattern = "log", replacement = "exp")
    rlist <- lapply(rlist, sub, pattern = "1p", replacement = "m1")
    rlist <- lapply(rlist, sub, pattern = "Mul", replacement = "Div")
    print(rlist)
    tmp <- df[, vars]
    for (i in seq_len(n)) {
        tmp <- as.data.frame(sapply(vars, function(x) do.call(rlist[[x]][i], 
            list(tmp[, x]))))
    }
    return(tmp)
}

tlist <- list("H"="log1p", "O"="log1p", "Charge"="Safelog", "Ba"="log1p",
              "Cl"="log1p", "S(6)"="log1p", "Sr"="log1p",
              "Barite"="log1p", "Celestite"="log1p")

preprocess <- function(df) {
    minmaxlog <- list(min = c(H = 4.71860987935512, O = 4.03435069501537, Charge = -14.5337693764617, 
                              Ba = 1.87312878574393e-141, Cl = 0, `S(6)` = 4.2422742065922e-07, 
                              Sr = 0.00049370806741832, Barite = 0.000999043199940672, Celestite = 0.218976382406766),
                      max = c(H = 4.71860988013054, O = 4.03439461483133, Charge = 12.9230752782909, 
                              Ba = 0.086989273200771, Cl = 0.178729802869381, `S(6)` = 0.000620582151722819, 
                              Sr = 0.0543631841661421, Barite = 0.56348209786429, Celestite = 0.69352422027466))
    if (!is.data.frame(df))
        df <- as.data.frame(df, check.names = FALSE)
    tlog <- TransfList(df, tlist)
    as.data.frame(lapply(colnames(tlog),
                         function(x) scale_min_max(x=tlog[x], min=minmaxlog$min[x], max=minmaxlog$max[x], backtransform=FALSE)),
                  check.names = FALSE)
}

postprocess <- function(df) {
    minmaxlog <- list(min = c(H = 4.71860987935512, O = 4.03435069501537, Charge = -14.5337693764617, 
                              Ba = 1.87312878574393e-141, Cl = 0, `S(6)` = 4.2422742065922e-07, 
                              Sr = 0.00049370806741832, Barite = 0.000999043199940672, Celestite = 0.218976382406766),
                      max = c(H = 4.71860988013054, O = 4.03439461483133, Charge = 12.9230752782909, 
                              Ba = 0.086989273200771, Cl = 0.178729802869381, `S(6)` = 0.000620582151722819, 
                              Sr = 0.0543631841661421, Barite = 0.56348209786429, Celestite = 0.69352422027466))
    if (!is.data.frame(df))
        df <- as.data.frame(df, check.names = FALSE)
    
    ret <- as.data.frame(lapply(colnames(df),
                                function(x) scale_min_max(x=df[x], min=minmaxlog$min[x], max=minmaxlog$max[x], backtransform=TRUE)),
                         check.names = FALSE)

    BackTransfList(ret, tlist)
}